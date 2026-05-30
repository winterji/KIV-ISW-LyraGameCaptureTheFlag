# 05 — Komentovaný příklad: Listener eliminace

Tento dokument je průchozí ukázkou jedné funkce, která už v `B_ISW_AI` existuje: listeneru na `Lyra.Elimination.Message`. Je to nejčistší příklad v projektu na vzor *vjem skrz herní zprávu*, a ukazuje, jak udržet bázi představ agenta konzistentní, když se svět mění mimo jeho vizuální vnímání.

Cíl: když relevantní pawn umře, agent aktualizuje svůj blackboard tak, aby behavior tree mohl zareagovat na příštím ticku.

V Kubíkově rámci je herní zpráva hybridem **reaktivní komunikace** ([Kubík 2004, Kapitola 4.2.1] — stigmergické stopy v prostředí) a **komunikačního aktu** ([Kubík 2004, Kapitola 4.4] — explicitní pub/sub zprávy ve vyšším jazyce). Hra vysílá typovanou událost s jasnou sémantikou; agenti, kteří se přihlásili k odběru, ji zpracovávají jako `vjem`. Důležité: tato percepce obchází `AIPerceptionComponent` — agent na druhé straně mapy se o eliminaci dozví, i když ji nemohl vidět. Je to stejný kompromis, o kterém Kubík mluví na s. 17 mezi stigmergií a přímou komunikací: stigmergie je realistická, ale pomalá a ztrátová; přímá komunikace je všeznalá, ale omezená na události, které systém explicitně emituje.

## Co listener má dělat

Relevantní jsou dvě situace:

1. **Náš agent sám zemře.** Bez ohledu na to, zda nesl vlajku, musíme vyčistit `CarryingFlag` a `WePickedUpFlag`, aby — až tento controller `OnPossess`ne dalšího pawna po respawnu — BT nevěřil, že stále nese.
2. **Nepřítel, který právě nese naši vlajku, zemře.** Musíme vyčistit `OurFlagCaptured` a `FlagCarrier`, aby BT opustil větev „vrátit naši vlajku" a vrátil se k tomu, co dělal předtím.

Tyto dvě kontroly jsou nezávislé. Jedna eliminace může být obě (bot sám je nositel vlajky, který umřel). Handler je spouští postupně bez zkratu mezi nimi.

## Kde listener žije

V `B_ISW_AI`, v `BeginPlay → AsyncAction_ExperienceReady → OnReady`. Konkrétně: ve stejné fázi, která objevuje podstavce vlajek, po nastavení podstavců na blackboardu.

**Proč ne v `OnPossess`?** `OnPossess` se spouští při každém respawnu. Pokud byste tam listener registrovali, hromadili byste duplicitní listenery jeden na každý život — třetí smrt by handler spustila třikrát na každou eliminaci, atd.

Controller objekt přežívá possessions, takže registrace jednou v `BeginPlay` je přesně správně.

## Handler krok po kroku

V Blueprintu je listener uzel `Listen for Gameplay Messages` nakonfigurovaný takto:

- **Channel:** `Lyra.Elimination.Message`
- **Payload Type:** `FLyraVerbMessage`

Jeho „On Message Received" exec drát vede do malého podgrafu, který provádí dvě kontroly níže.

### Krok 0 — Stínění blackboardu

```
IsValid(BBComp) → Branch
  └── False: return  (obranné — po OnPossess by se to nemělo stát, ale levné)
  └── True:  pokračuj
```

`BBComp` je cachovaný `Blackboard Component`, který jsme uložili z `UseBlackboard`. Viz [03 — Architektura AI](03-AI-Architecture.md) proč na tom záleží.

### Krok 1 — Umřel náš agent?

Porovnáváme eliminovaného hráče s **naší** PlayerState:

```
Self (AI Controller) → GetPlayerState        ← PS controlleru, ne pawnu
Payload.Target                                 ← už PlayerState
==
Branch:
  True:
    BBComp → SetValueAsBool("CarryingFlag",   false)
    BBComp → SetValueAsBool("WePickedUpFlag", false)
```

**Proč `GetPlayerState` z controlleru a ne přes `GetControlledPawn → GetPlayerState`?** V okamžiku, kdy tato zpráva přijde, pawn už může být zničený (sice umřel, že). Controller přežívá smrt pawnu a drží platnou referenci na PlayerState. Cesta přes pawn by dereferencovala None.

**Proč jen tyto dva klíče?** Až další pawn spawne a my ho `OnPossess`neme, tohle jsou per-life představy „nesu nepřátelskou vlajku", které musí být false. Statické představy (pozice základen) se netýkají. Sdílené představy (`OurFlagCaptured`, `FlagCarrier`) obstará krok 2.

### Krok 2 — Umřel nepřítel, který nesl *naši* vlajku?

Porovnáváme eliminovaného hráče s nositelem, o kterém věříme, že drží naši vlajku:

```
BBComp → GetValueAsObject("FlagCarrier") → As Pawn → GetPlayerState
Payload.Target
==
Branch:
  True:
    BBComp → SetValueAsObject("FlagCarrier",   None)
    BBComp → SetValueAsBool("OurFlagCaptured", false)
```

Pár malých věcí:

- `GetValueAsObject` vrací referenci typu `Object`. Castněte na `Pawn` (nebo `LyraCharacter`) než zavoláte `GetPlayerState`. Pokud cast selže, propadněte dál — nositel buď nebyl nikdy nastaven, nebo už byl vyčištěn.
- Porovnáváme `PlayerState` s `PlayerState`. Payload `Target` je PS; nositel, kterého máme uloženého, je pawn, jehož PS dereferencujeme. Oba musí být stejného typu, aby porovnání mělo smysl.
- Když je nositel vyčištěn, dekorátor `OurFlagCaptured == true` v BT přejde na false na příštím ticku. Díky **Observer Aborts: Lower Priority**, které jsme na ten dekorátor nastavili v [04 — Blackboard a Behavior Tree](04-Blackboard-and-BehaviorTree.md), podstrom vrácení vlajky okamžitě skončí a agent se vrátí buď ke krádeži nebo k nesení.

### Krok 3 — Pořadí, ne zkrat

Obě kontroly se vždy provedou. I když byl krok 1 true, pořád testujeme krok 2 — protože agent sám mohl být nositelem vlajky (vlajku nepřítele předtím ukradl a vracel se domů). Pokud byste mezi nimi zkracovali, nechali byste `FlagCarrier` ukazovat na mrtvý pawn a chování spoluhráčů odkazujících se na něj by bylo zmatené.

## Co se agent z této zprávy nemůže dozvědět

Stojí za pauzu, protože studenti někdy přeceňují, co jim herní zprávy říkají:

- Zpráva vám neřekne, zda eliminovaný byl ze stejného týmu. Musíte si to odvodit sami (např. porovnat `FindTeamFromObject(Payload.Target)` s vlastním týmem).
- Neřekne vám *místo* zabití — musíte si ho odvodit z pawnu nebo si ho přečíst dřív, než pawn zanikne.
- Je **server-side only**. Client-only kód (jako UI) ji nemůže použít bez replikované property nebo RPC. AI controllery žijí na serveru, takže nám to vyhovuje.

## Proč je tohle vzor hodný kopírování

Listener eliminace je malý, ale procvičuje každé důležité pravidlo:

- Listener registrovaný přesně jednou, v životní fázi, která přežije respawn (`BeginPlay`, ne `OnPossess`).
- Přístup k blackboardu stíněný `IsValid(BBComp)` a vedený přes cachovanou komponentu.
- Kontrola identity vede přes `Controller → GetPlayerState`, nikdy přes (možná mrtvý) pawn.
- Dvě nezávislé aktualizace stavu provedené postupně, ne vnořené v `else`.
- Handler aktualizuje **představy**, ne chování. Mění hodnoty blackboardu; nevolá `MoveTo`, `StopMovement`, ani nic jiného. BT rozhodne, co dělat, na svém příštím ticku na základě nových představ.

Tento poslední bod je koncepčně nejdůležitější. V Kubíkově formálním modelu handler zprávy implementuje funkci `změna_stavu : P × I → I` — transformuje přicházející vjem na nový vnitřní stav. Funkce výběru akce `akce : P × I → A` patří behavior treeu. Mísení obou (např. volání `MoveTo` z handleru zprávy) ruší oddělení, které agenta dělá debugovatelným: ztrácíte schopnost ptát se „za předpokladu, že agent věří X, co by udělal?", protože chování už není funkcí samotného stavu.

Tento tvar zkopírujte pro libovolnou další herní zprávu, na kterou chcete reagovat. Dvě vlajkové zprávy (`Lyra.CaptureTheFlag.FlagPickedUp.Message` a `Lyra.CaptureTheFlag.FlagDelivered.Message`) se řídí stejnou šablonou — jsou už implementované v `B_ISW_AI` a můžete je číst jako posílení vzoru.
