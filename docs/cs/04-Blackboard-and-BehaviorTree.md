# 04 — Blackboard a Behavior Tree

Tento dokument je referencí baseline agentovy **báze představ** a jeho **funkce výběru akce**.

## Blackboard: `BB_ISW_Bot`

Blackboard je báze představ agenta. V termínech formálního modelu je to `I` — množina vnitřních stavů. Každý klíč reprezentuje jednu představu, kterou agent právě o světě má. `BB_ISW_Bot` používá tyto klíče:

### Statické představy (nastavené jednou za zápas)

| Klíč                       | Typ    | Nastaveno v                            | Význam                                               |
| -------------------------- | ------ | -------------------------------------- | ---------------------------------------------------- |
| `OwnFlagBaseLocation`      | Vector | `BeginPlay` (po ExperienceReady)       | Pozice podstavce vlastní vlajky ve world space.      |
| `EnemyFlagBaseLocation`    | Vector | `BeginPlay` (po ExperienceReady)       | Pozice podstavce nepřátelské vlajky ve world space.  |
| `SelfActor`                | Object | `OnPossess`                            | Reference na pawnu tohoto bota. Šikovné pro tasky.   |

### Volatilní sdílené představy (aktualizované herními zprávami nebo službami)

| Klíč              | Typ    | Aktualizuje            | Význam                                                                     |
| ----------------- | ------ | ------------------- | -------------------------------------------------------------------------- |
| `CarryingFlag`    | Bool   | Listenery pickup / eliminace  | Tento bot právě nese nepřátelskou vlajku.                                  |
| `WePickedUpFlag`  | Bool   | Listenery pickup / eliminace  | Můj tým nese nepřátelskou vlajku. |
| `OutOfAmmo`       | Bool   | Self                | Označuje, zda má agent ještě náboje. |
| `TargetEnemy`     | Object | AIPerception        | Právě vnímaný nepřátelský pawn.                                             |
| `MoveGoal`        | Vector | BT tasky            | Aktuální cíl pohybu. Generický klíč destinace.                              |
| `OurFlagCaptured` | Bool   | Listenery pickup / eliminace      | Nepřítel právě nese naši vlajku.                                   |
| `EnemyFlagCarrier`     | Object | Listenery pickup / eliminace      | Pawn, který právě nese naši vlajku (platné jen když `OurFlagCaptured == true`). |
| `OwnFlagCarrier`     | Object | Listenery pickup / eliminace      | Pawn, který právě nese nepřátelskou vlajku (platné jen když `WePickedUpFlag == true`). |
| `CanSeeFlagCarrier`  | Bool   | `BTS_CheckLOS` (služba)   | Výsledek testu přímé viditelnosti `LineOfSightTo(FlagCarrier)`.    |

### Poznámky k tabulce

- `OwnFlagBaseLocation` / `EnemyFlagBaseLocation` jsou **nastavené jednou a nikdy se nemění**. Pokud chybí, znamená to, že `BeginPlay` předběhl přiřazení do týmu.

## Behavior Tree: `BT_ISW_CTF_bot`

Baseline strom je Selector v rootu, který se větví podle aktuálních beliefs agenta:

```
Root Selector
├── [OutOfAmmo == false]  → má náboje, může střílet
├── [OutOfAmmo    == true]  → Nemá náboje, hledá zbraň
```

Když je `OutOfAmmo` false, znamená, že agent může střílet, takže je mu nastavena logika hry. 

```
[OutOfAmmo == false] → má náboje, může střílet
├── [CarryingFlag == true] → nesu vlajku, chci ji donést
├── [OurFlagCaptured == true] → ukradli nám vlajku, chci ji získat zpět
├── [WePickedUpFlag == true] → neseme vlajku, bráním nositele
├── [Default] → jdu si pro vlajku a bojuji
```

Každá větev popsána dále.

### Větev 1 — Návrat na základnu

Aktivní, když tento bot nese nepřátelskou vlajku.

```
Sequence [Dekorátor: CarryingFlag == true]
  └── MoveTo(OwnFlagBaseLocation)
```

**Záměrně minimalistické, aby mohlo být rozšířeno.**

### Větev 2 — Vrácení naší vlajky

Aktivní, když **naše** vlajka byla ukradena a my víme, kdo ji nese.

```
Simple Parallel [Dekorátor: OurFlagCaptured == true]
[BTS_CheckLOS služba]
  ├── (Main task) Move To [EnemyFlagCarrier]
  └── (parallel task )Selector
        ├── Sequence [Dekorátor: CanSeeFlagCarrier == true]
            └── [Lyra Shooting Service připojená na této Sequence]
```

Důležitá poznámka:

**Lyra Shooting Service.** Shooting service je dodávaná Lyrou a sdílená napříč stock botem. Připojte na sequence, kde je nastavený target na koho střílet (a target na který vidí - `BTS_CheckLOS` nebo `AIPerception`). 


### Větev 3 — Krádež

Default větev, když neplatí ani jedno z předchozího.

```
MoveTo(EnemyFlagBaseLocation)
```

Ještě jednodušší. Nositel nevybírá cesty inteligentně; neútočí oportunisticky; nestahuje se, když je v přesile. Tohle všechno je záměrná rezerva.

## Vlastní služby a tasky dodané s projektem

- **`BTS_CheckLOS`** — jednou za tick interval provede `LineOfSightTo(FlagCarrier)` z controlleru a zapíše výsledek do `CanSeeFlagCarrier`.

<!-- AI Generated, Not tested

## Čtení stromu v editoru

Otevřete `BT_ISW_CTF_bot` v editoru a spusťte hru (PIE). Pak:

- Právě běžící uzel je žlutě zvýrazněný.
- Pravým klikem na libovolný uzel v běžícím stromě si zobrazíte hodnoty jeho blackboardu.
- **Gameplay Debugger** (`'` v PIE) ukazuje aktuální BB a aktuální task na obrazovce pro vybraného bota.

Tato živá introspekce je zdaleka nejrychlejší způsob, jak pochopit existující strom před jeho úpravou. -->
