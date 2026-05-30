# 04 — Blackboard a Behavior Tree

Tento dokument je referencí baseline agentovy **báze představ** (v Kubíkových termínech: množina vnitřních stavů `I` z formálního modelu) a jeho **funkce výběru akce** (`akce : P × I → A`).

## Blackboard: `BB_ISW_Bot`

Blackboard je báze představ agenta. V termínech formálního modelu je to `I` — množina vnitřních stavů. Každý klíč reprezentuje jednu představu, kterou agent právě o světě má. `BB_ISW_Bot` používá tyto klíče:

### Statické představy (nastavené jednou za zápas)

| Klíč                       | Typ    | Nastaveno v                            | Význam                                               |
| -------------------------- | ------ | -------------------------------------- | ---------------------------------------------------- |
| `OwnFlagBaseLocation`      | Vector | `BeginPlay` (po ExperienceReady)       | Pozice podstavce vlastní vlajky ve world space.      |
| `EnemyFlagBaseLocation`    | Vector | `BeginPlay` (po ExperienceReady)       | Pozice podstavce nepřátelské vlajky ve world space.  |
| `SelfActor`                | Object | `OnPossess`                            | Reference na pawnu tohoto bota. Šikovné pro tasky.   |

### Per-life představy (resetované při každém `OnPossess`)

| Klíč              | Typ    | Vlastník            | Význam                                                                     |
| ----------------- | ------ | ------------------- | -------------------------------------------------------------------------- |
| `CarryingFlag`    | Bool   | Self                | Tento bot právě nese nepřátelskou vlajku.                                  |
| `WePickedUpFlag`  | Bool   | Self                | Tento bot vlajku vyzvedl (někdy ekvivalentní `CarryingFlag`, sledováno odděleně pro detekci *události* vyzvednutí oproti *stavu* nesení). |
| `OutOfAmmo`       | Bool   | Self                | Samozřejmé; rezervováno pro chování citlivá na munici, která student doplní. |
| `TargetEnemy`     | Object | AIPerception        | Právě vnímaný nepřátelský pawn.                                             |
| `MoveGoal`        | Vector | BT tasky            | Aktuální cíl pohybu. Generický klíč destinace.                              |

### Volatilní sdílené představy (aktualizované herními zprávami nebo službami)

| Klíč                 | Typ    | Aktualizuje                       | Význam                                                             |
| -------------------- | ------ | --------------------------------- | ------------------------------------------------------------------ |
| `OurFlagCaptured`    | Bool   | Listenery pickup / eliminace      | Nepřítel právě nese naši vlajku.                                   |
| `FlagCarrier`        | Object | Listenery pickup / eliminace      | Pawn, který právě nese naši vlajku (platné jen když `OurFlagCaptured == true`). |
| `CanSeeFlagCarrier`  | Bool   | `BTS_CheckLOS` (služba)           | Výsledek testu přímé viditelnosti `LineOfSightTo(FlagCarrier)`.    |

### Čtení tabulky

Pár invariantů ke zapamatování:

- `OwnFlagBaseLocation` / `EnemyFlagBaseLocation` jsou **nastavené jednou a nikdy se nemění**. Pokud chybí, znamená to, že `BeginPlay` předběhl přiřazení do týmu — stínete `bIsPartOfTeam`.
- `OurFlagCaptured` a `FlagCarrier` jsou svázané. Vždycky je nastavujte nebo čistěte společně. Nastavení `OurFlagCaptured` bez nastavení `FlagCarrier` nechává BT v nekonzistentním stavu.
- `CanSeeFlagCarrier` má smysl **jenom když `FlagCarrier != None`**. Služba by měla brzo skončit, pokud je nositel neplatný.

## Behavior Tree: `BT_ISW_CTF_bot`

Baseline strom je Selector v rootu, který se větví podle aktuální role agenta:

```
Root Selector
├── [OurFlagCaptured == true]  → Brání / vrací naši vlajku
├── [CarryingFlag    == true]  → Vrací se na základnu s vlajkou
├── [Default]                  → Krade: naviguje na nepřátelskou základnu
```

Každá větev popsána dále.

### Větev 1 — Vrácení naší vlajky

Aktivní, když **naše** vlajka byla ukradena a my víme, kdo ji nese.

```
Sequence [Dekorátor: OurFlagCaptured == true]
  ├── [BTS_CheckLOS služba]
  └── Selector
      ├── Sequence [Dekorátor: CanSeeFlagCarrier == true, Observer Aborts: Lower Priority]
      │   └── [Lyra Shooting Service připojená na této Sequence]
      │       └── Simple Parallel (Finish: Immediate)
      │           ├── Main: MoveTo(FlagCarrier)
      │           └── Background: Wait(9999)
      └── MoveTo(FlagCarrier)          ← pronásledování bez střelby
```

Dva důležité vzory:

**LOS brána Lyra Shooting Service.** Shooting service je dodávaná Lyrou a sdílená napříč stock botem. Důrazně se nedoporučuje ji upravovat. Místo toho ji připojte na sequence, jejíž dekorátor (`CanSeeFlagCarrier == true`) určuje, zda střelba může probíhat. Když dekorátor přejde na false, podstrom přeruší a selector se propadne na pronásledovací větev. Střelba je tak reaktivní vůči vjemu bez jakýchkoliv změn v Lyra kódu.

Funkčně to je totéž jako Brooksův mechanismus **potlačení a zabránění** mezi vrstvami v subsumpční architektuře [Kubík 2004, Kapitola 1.3.2]: podstrom s vyšší prioritou může inhibovat nižší, když je jeho aktivační podmínka splněna. U Brooksových robotů to bylo zapojeno explicitními signálovými odbočkami; v BT je to zapojeno dekorátorem a Observer Aborts.

**Proč Simple Parallel + Wait(9999)?** Toto je doporučený Lyra vzor pro „prováděj movement task, zatímco služba dál tikuje". `MoveTo` by jinak ukončilo podstrom, jakmile uspěje; parallel ho drží naživu (a shooting service dál tikuje), dokud dekorátor agentův názor nezmění.

**Proč Observer Aborts: Lower Priority na dekorátoru, ne uvnitř parallelu?** Empiricky, observer aborts umístěné **uvnitř** background větve Simple Parallelu jsou nespolehlivé — abort signál se ne vždy propaguje ven. Vždy dejte rozhodující dekorátor na Selector *nad* parallel.

### Větev 2 — Návrat na základnu

Aktivní, když tento bot nese nepřátelskou vlajku.

```
Sequence [Dekorátor: CarryingFlag == true]
  └── MoveTo(OwnFlagBaseLocation)
```

Záměrně minimalistické. Vylepšení této větve je jedno z očividných zadání pro studenty — nositel teď jde domů deterministickou přímkou.

### Větev 3 — Krádež

Záložní větev, když neplatí ani jedno z předchozího.

```
MoveTo(EnemyFlagBaseLocation)
```

Ještě jednodušší. Nositel nevybírá cesty inteligentně; neútočí oportunisticky; nestahuje se, když je v přesile. Tohle všechno je záměrná rezerva.

## Vlastní služby a tasky dodané s projektem

- **`BTS_CheckLOS`** — jednou za tick interval provede `LineOfSightTo(FlagCarrier)` z controlleru a zapíše výsledek do `CanSeeFlagCarrier`. Používá `GetOwnersBlackboard → SetValueAsBool` s názvem klíče jako stringem. (Uvnitř služeb a tasků `GetOwnersBlackboard` funguje; v controlleru ne — používejte cached `BBComp`.)

## Čtení stromu v editoru

Otevřete `BT_ISW_CTF_bot` v editoru a spusťte hru (PIE). Pak:

- Právě běžící uzel je žlutě zvýrazněný.
- Pravým klikem na libovolný uzel v běžícím stromě si zobrazíte hodnoty jeho blackboardu.
- **Gameplay Debugger** (`'` v PIE) ukazuje aktuální BB a aktuální task na obrazovce pro vybraného bota.

Tato živá introspekce je zdaleka nejrychlejší způsob, jak pochopit existující strom před jeho úpravou.
