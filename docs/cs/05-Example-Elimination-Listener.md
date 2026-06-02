# 05 — Komentovaný příklad: Listener vyzvednutí vlajky

Tento dokument je průchozí ukázkou jedné funkce, která už v `B_ISW_AI` existuje: listeneru na `Lyra.CaptureTheFlag.FlagPickedUp.Message`. Je to nejčistší příklad v projektu na vzor *vjem skrz herní zprávu*, a ukazuje, jak agent v jediném okamžiku aktualizuje hned více vzájemně provázaných představ, než aby přiřazení rozbíjel do ad-hoc podmínek roztroušených po behavior treeu.

Cíl: když někdo zvedne vlajku, agent aktualizuje svůj blackboard tak, aby behavior tree mohl zareagovat na příštím ticku.

V Kubíkově rámci je herní zpráva hybridem **reaktivní komunikace** ([Kubík 2004, Kapitola 4.2.1] — stigmergické stopy v prostředí) a **komunikačního aktu** ([Kubík 2004, Kapitola 4.4] — explicitní pub/sub zprávy ve vyšším jazyce). Hra vysílá typovanou událost s jasnou sémantikou; agenti, kteří se přihlásili k odběru, ji zpracovávají jako `vjem`. Důležité: tato percepce obchází `AIPerceptionComponent` — agent na druhé straně mapy se o vyzvednutí dozví, i když ho nemohl spatřit. Je to kompromis, o kterém Kubík mluví na s. 17 mezi stigmergií a přímou komunikací: stigmergie je realistická, ale pomalá a ztrátová; přímá komunikace je všeznalá, ale omezená na události, které systém explicitně emituje.

## Payload: `LyraFlagStatusMessage`

Na rozdíl od listeneru eliminace, který přijímá `LyraVerbMessage`, tento listener pracuje s vlastní strukturou `LyraFlagStatusMessage` definovanou v projektu:

| Pole          | Typ      | Obsah                                                          |
| ------------- | -------- | -------------------------------------------------------------- |
| `Instigator`  | `APawn*` | Pawn, který vlajku zvedl.                                      |
| `FlagTeamId`  | `int32`  | Týmové ID **vlajky**, která byla zvednuta (ne toho, kdo zvedl). |
| `Pad`         | `AActor*`| Podstavec, ze kterého vlajka pocházela.                        |

Nejdůležitější detail: `FlagTeamId` je tým *vlajky*, ne útočníka. Pokud nepřítel zvedne naši vlajku, `FlagTeamId` bude naše týmové ID.

## Co listener má dělat

Relevantní jsou tři situace:

1. **Nepřítel zvedl naši vlajku.** Musíme nastavit `OurFlagCaptured = true` a `FlagCarrier = Instigator`, aby BT přešel do větve „vrátit naši vlajku" a věděl, na koho se zaměřit.
2. **Spoluhráč (nebo my sami) zvedl nepřátelskou vlajku.** Musíme nastavit `WePickedUpFlag = true`, aby BT přešel do větve „bránit nositele".
3. **My sami jsme zvedli nepřátelskou vlajku.** Navíc k bodu 2 musíme nastavit `CarryingFlag = true`, aby BT přešel do větve „donést vlajku domů".

Situace 2 a 3 nejsou výlučné — případ 3 vždy implikuje případ 2. Handler je spouští postupně bez zkratu.

## Handler krok po kroku

V Blueprintu je listener uzel `Listen for Gameplay Messages` nakonfigurovaný takto:

- **Channel:** `Lyra.CaptureTheFlag.FlagPickedUp.Message`
- **Payload Type:** `LyraFlagStatusMessage`

Jeho „On Message Received" exec drát vede do malého podgrafu, který provádí tři kontroly níže.


### Krok 1 — Zvedl nepřítel naši vlajku?

Porovnáme `FlagTeamId` zprávy s naším vlastním týmem a ověříme, zda nositel je nepřítel:

```
Self (AI Controller) → Get Controlled Pawn → FindTeamFromObject → TeamId
Payload.FlagTeamId
==
Branch:
  True:
    (FlagTeamId == náš tým → naše vlajka byla zvednuta)
    Blackboard → SetValueAsObject("EnemylagCarrier", Payload.Instigator)
    Blackboard → SetValueAsBool("OurFlagCaptured", true)
```

![pickup_flag_1](../img/pickup_flag_1.png)


**Proč kontrolujeme `FlagTeamId` a ne tým instigatora?** Zpráva reportuje, *čí* vlajka byla zvednuta. Instigator je útočník — porovnání jeho týmu by logiku otočilo. `FlagTeamId == náš tým` přesně říká: naše vlajka je v rukou nepřítele.

**Proč ukládáme `Instigator` přímo, ne jeho PlayerState?** `FlagCarrier` je `Object` klíč a `BTS_CheckLOS` pracuje s pawn referencí pro `LineOfSightTo`. Ukládáme pawn. Pokud budete potřebovat PlayerState (např. pro jiné porovnání), derivujte ji z pawnu v místě použití — v okamžiku zvednutí je pawn zaručeně živý.

### Krok 2 — Zvedl spoluhráč nepřátelskou vlajku?

Použijeme stejné porovnání akorát budeme reagovat na false místo true.

```
==
Branch:
  False:
    (nositel je ze stejného týmu → neseme nepřátelskou vlajku)
    Blackboard → SetValueAsBool("WePickedUpFlag", true)
    Blackboard → SetValueAsObject("OurFlagCarrier", Payload.Instigator)
```

`WePickedUpFlag` je signál pro BT, aby přešel do větve „chránit nositele vlajky". Platí pro celý tým, ne jen pro tohoto agenta.

### Krok 3 — Jsem to já, kdo vlajku zvedl?

Porovnáme instigatora s naším kontrolovaným pawnem (na screenshotu je to ta horní operace equals):

```
Self (AI Controller) → GetControlledPawn
Payload.Instigator
==
Branch:
  True:
    Blackboard → SetValueAsBool("CarryingFlag", true)
```

**Proč `GetControlledPawn` a ne `GetPlayerState` porovnání?** Instigator je `APawn*`. Porovnání pawn s pawn je přímé. V okamžiku, kdy zpráva přijde, pawn je naživu a reference je platná.

## Co se agent z této zprávy nemůže dozvědět

Stojí za pauzu, protože studenti někdy přeceňují, co jim herní zprávy říkají:

- Zpráva vám neřekne, kde přesně k vyzvednutí došlo — pokud potřebujete pozici, čtěte ji z `Payload.Pad` nebo `Payload.Instigator` v handleru, dokud jsou platné.
- Neřekne vám, zda nositel je viditelný — to obstarává `BTS_CheckLOS` služba za běhu.
- Je **server-side only**. Client-only kód (jako UI) ji nemůže použít bez replikované property nebo RPC. AI controllery žijí na serveru, takže nám to vyhovuje.
