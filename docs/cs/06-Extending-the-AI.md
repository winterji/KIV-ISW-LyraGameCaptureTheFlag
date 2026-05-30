# 06 — Rozšiřování AI

Tento dokument prochází nejčastější změny, které budete dělat. Pro každou uvádí bezpečný postup a vyjmenovává chyby, na které studenti v minulosti naráželi.

## Přidání nového klíče na blackboard

1. Otevřete `Content/Bot/BB_ISW_Bot`.
2. Klikněte **New Key** a vyberte typ. Bool, Vector, Object a Float pokrývají skoro všechno.
3. Pojmenujte ho podle **toho, co znamená**, ne podle toho, odkud pochází. `EnemyIsNearMyBase` je dobré; `PerceptionEvent42` špatné.
4. Doplňte řádek do tabulky v [04 — Blackboard a Behavior Tree](04-Blackboard-and-BehaviorTree.md), aby budoucí studenti věděli, co jste přidali.

**Časté chyby:**

- Pokud nový klíč čtete z BT dekorátoru dřív, než do něj cokoli zapíšete, dostanete defaultní hodnotu typu (false / nula / None). To je obvykle v pořádku — pokud ne, zapište ho v `OnPossess`.
- Pokud klíč chcete číst z BT služeb / tasků přes `GetOwnersBlackboard → GetValueAsX`, název klíče se předává **jako string**. Překlep v názvu tiše vrátí default. Používejte jednu pojmenovanou string konstantu, pokud to jde.

## Psaní nové BT služby

Služby běží, dokud je jejich podstrom aktivní. Použijte je pro udržování odvozené představy (např. „je nositel viditelný") nebo pro levný polling.

1. V Content Browseru pravý klik → **AI → Behavior Tree Service**.
2. Otevřete. Override `Event Receive Tick` (nebo `Event Receive Tick AI`, pokud chcete mít `Controlled Pawn` už zapojený).
3. Z `GetOwnersBlackboard` zapisujte do klíče, který chcete aktualizovat. **Název klíče jako string** — `SetValueAsBool(KeyName="MyKey", Value=...)`.
4. Položte službu na uzel v BT. Vylaďte tick interval (default 0.5 s s náhodnou odchylkou obvykle stačí).

**Časté chyby:**

- `GetOwnersBlackboard` funguje jen uvnitř BT uzlů. Nepoužívejte ho z controlleru.
- Služba tikající 60× za sekundu skoro nikdy není potřeba. Používejte intervaly.
- Pokud služba dělá line trace, preferujte Lyrovo `LineOfSightTo` před surovým `LineTraceByChannel` — už respektuje nastavení percepčních kanálů.
- Nepoužívejte službu pro to, aby *něco dělala*. Služby aktualizují představy. Tasky dělají.

## Psaní nové BT task

Task je konkrétní akce: pohnout se někam, vystřelit, vyměnit zbraň, upustit vlajku, atd. Tasky uspějí, selžou, nebo zůstávají běžící (pro „in-progress" tasky).

1. Content Browser → pravý klik → **AI → Behavior Tree Task**.
2. Override `Event Receive Execute AI`.
3. Dělejte svou práci. Na konci zavolejte `Finish Execute` s `Success` nebo `Fail`. Pro dlouhotrvající task (pohyb, animace) nevolejte `Finish Execute` hned — uložte si stav na tasku a zavolejte z callbacku (např. `OnMoveCompleted`).
4. Položte task jako list v BT. Nakonfigurujte exposed parametry v details panelu BT.

**Časté chyby:**

- Zapomenuté `Finish Execute` nechá BT zaseknutý na vašem tasku navždy. Gameplay debugger ukáže váš task jako aktivní a nic jiného neběží.
- Pokud zavoláte `Finish Execute` synchronně ve stejném frame, kdy task začal, chování nad ním nemusí přechod zaznamenat — vzácné, ale stává se. Učebnicová oprava je delay jeden frame.
- Nezapisujte *behavior-wide* stav do instance proměnných na tasku. Tasky mohou být re-entered. Pro stav, který musí přetrvávat, používejte blackboard.

## Přidání nového listeneru herní zprávy

Použijte [komentovaný příklad](05-Example-Elimination-Listener.md) jako šablonu. Souhrn pravidel:

- Registrace v `BeginPlay → AsyncAction_ExperienceReady → OnReady`, ne v `OnPossess`.
- V handleru identifikujte „self" přes `GetPlayerState` controlleru, ne pawnu.
- Stínete `IsValid(BBComp)` před libovolným přístupem k blackboardu.
- Aktualizujte představy, ne chování. Nevolejte `MoveTo` z handleru zprávy.

## Tvorba vlastního controlleru

Váš agent je **potomkem Blueprintu `B_ISW_AI`** — `B_ISW_AI` přímo neupravujete. Je to sdílený baseline, na kterém staví všichni studenti.

### Krok 1 — Vytvořte child Blueprint

1. V Content Browseru vytvořte složku `Content/Bot/Student_<jmeno>/`.
2. Pravý klik na `Content/Bot/B_ISW_AI` → **Create Child Blueprint Class**.
3. Pojmenujte ho `B_<jmeno>_AI` a uložte do své složky.

Co dostanete zdarma zděděním:

- Kompletní `BeginPlay` životní cyklus: ExperienceReady gating, objevení podstavců vlajek a stávající listenery herních zpráv.
- Handler `OnTargetPerceptionUpdated`, který zapisuje `TargetEnemy` na blackboard.
- Tři listenery herních zpráv: vyzvednutí vlajky, doručení vlajky a eliminace (viz [05 — Listener eliminace](05-Example-Elimination-Listener.md)).
- Vzor cachování `BBComp` a všechny per-life resety v `OnPossess`.

### Krok 2 — Vytvořte vlastní Behavior Tree

1. Content Browser → pravý klik → **AI → Behavior Tree**. Pojmenujte `BT_<jmeno>_bot`.
2. V Details panelu BT nastavte **Blackboard Asset** na `BB_ISW_Bot`. Blackboard můžete rozšířit — viz „Přidání nového klíče na blackboard" výše.
3. Stavějte strom. Dostupné jsou všechny existující typy uzlů (`BTS_CheckLOS`, `MoveTo`, Lyra shooting service).

### Krok 3 — Override `OnPossess` pro spuštění vašeho stromu

Overridujte `OnPossess` ve svém child Blueprintu. **Nejdříve zavolejte rodiče** (`Parent: On Possess`) — to spustí cachování BB a per-life resety klíčů. Pak zavolejte `RunBehaviorTree` s vaším stromem:

```
Event OnPossess (InPawn)
  └── Parent: On Possess               ← cachuje BBComp, resetuje per-life klíče
  └── RunBehaviorTree(BT_<jmeno>_bot)
```

**Nevolejte `UseBlackboard` znovu** — rodič ho už zavolal a `BBComp` je teď platný.

### Krok 4 — Přidání nových listenerů herních zpráv

Chcete-li reagovat na události, které baseline nezpracovává (např. vlastní reakce na smrt, event munice, týmové skórování), overridujte `BeginPlay` ve svém child Blueprintu:

1. **Nejdříve zavolejte rodiče** (`Parent: Begin Play`) — to zaregistruje tři stávající listenery.
2. Přidejte vlastní uzly `Listen for Gameplay Messages` ve stejném pokračování `OnReady`, nebo napojte druhý řetěz `AsyncAction_ExperienceReady`.

Listenery rodiče zůstávají aktivní — přidáváte nové, ne je nahrazujete. Platí stejná pravidla: registrujte v `BeginPlay`, ne v `OnPossess`; stíněte `IsValid(BBComp)`; aktualizujte jen představy.

### Krok 5 — Nasměrujte experience na váš controller

1. Otevřete `Content/System/B_ShooterGame_CaptureTheFlag`.
2. Najděte položku **Bot** a změňte **AI Controller Class** na váš `B_<jmeno>_AI`.

To je jediná změna potřebná k tomu, aby každý bot v experience používal vaši třídu.

**Časté chyby specifické pro dědičnost:**

- **Override `OnPossess` bez volání rodiče.** `BBComp` se nikdy nezachytí; každý zápis na blackboard ve vašem kódu tiše selže.
- **Override `BeginPlay` bez volání rodiče.** Tři stávající listenery se nezaregistrují. Handler eliminace nevyčistí `FlagCarrier`; BT bude donekonečna pronásledovat mrtvý pawn.
- **Opětovné volání `UseBlackboard` ve vašem overridu.** Rodič ho už spustil. Opětovné volání nezmění korektnost, ale vytvoří druhý odkaz a je matoucí. Používejte zděděný `BBComp`.

---

## Kooperativní chování

Pro projekty, které koordinují chování mezi boty (viz [Zadání B a G](07-Assignments.md)), se standardní přístupy mapují přímo na Kubíkovu Kapitolu 4.

### Centralizovaná koordinace

Jeden server-side aktor přiřazuje botům role při spawnu. Každý bot uloží svou roli do nového BB klíče `MyRole` a BT se větví podle ní.

1. Vytvořte aktor `B_TeamCoordinator` (nebo `WorldSubsystem`) v experience. Sleduje obsazené role.
2. V `BeginPlay → OnReady` každého bota volejte `TeamCoordinator → RegisterBot(self)`, obdržte přiřazenou roli a zapište ji do `BBComp → SetValueAsEnum("MyRole", Role)`.
3. V BT přidejte top-level **Selector**, jehož děti jsou podstromy `[MyRole == Attacker]` a `[MyRole == Defender]`.
4. Při respawnu (`OnPossess`) se znovu zaregistrujte — koordinátor může rebalancovat, pokud se slot uvolnil.

Toto je *přímý dozor* [Kubík 2004, Kapitola 4.2] — Mintzbergova centralizovaná koordinace. Jednoduché a deterministické.

### Decentralizovaná koordinace (kontraktační síť)

Boti vysílají dostupnost a podávají nabídky na otevřené role přes herní zprávy nebo sdílený stavový aktor. Žádný koordinátor. Mapuje se na Smithův protokol kontraktační sítě [Kubík 2004, Kapitola 4.3.2]. Více implementační práce, ale bohatší teoretický writeup.

### Čtení sdíleného stavu světa (stigmergie)

Každý bot má svůj vlastní blackboard — sdílení BB klíče mezi boty není možné. Pro sdílená fakta (např. „nese aktuálně někdo z týmu nepřátelskou vlajku?") použijte **BT service**, která každý tik polluje sdílený aktor a zapisuje odvozené lokální představy:

1. Čtěte ze sdíleného aktoru (Game State, `B_TeamCoordinator`, nebo podstavec vlajky).
2. V BT service `BTS_CheckTeamCarrying` pollujte sdílený aktor a zapisujte lokální klíč `bool TeamCarryingFlag`.
3. Tento klíč používejte v dekorátorech jako obvykle.

Toto je vzor *stigmergie* / *reaktivní komunikace* [Kubík 2004, Kapitola 4.2.1]: agenti čtou stopy zanechané v prostředí místo přímé výměny zpráv.

---

## Začít od nuly (pokročilé)

Pokud vaše zadání vyžaduje úplné odstranění BT a spuštění vlastní rozhodovací smyčky na `Tick`, implementaci GOAP planneru nebo InteRRaP vrstvené architektury (zadání G), podědíte `LyraPlayerBotController` přímo místo `B_ISW_AI`.

Pro všechna ostatní zadání **dědíte od `B_ISW_AI`**.

Pokud začínáte od nuly:

- Ručně replikujte rozdělení fází: ExperienceReady pro statický setup, `OnPossess` pro per-life setup.
- Ručně registrujte listenery herních zpráv v `BeginPlay`, ne v `OnPossess`.
- Zapojte `AIPerceptionComponent` sami, pokud potřebujete zrak/sluch.
- Nastavte `Default Pawn Class` v experience na Lyra postavu s vybavením, které potřebujete.
- Nasměrujte experience na vaši třídu controlleru (stejně jako Krok 5 výše).

## Reference častých chyb

Všechny jsou v paměti `ctf_ai_lessons`, ale stojí za zopakování na jednom místě:

- **`GetBlackboard()` vrací None před `RunBehaviorTree`.** Cachujte output `UseBlackboard`.
- **`FindTeamFromObject` vrací -1, pokud pawn ještě není zařazen do týmu.** Stínete `bIsPartOfTeam`. Pokud false, `Delay 0.1` a zkuste znovu.
- **`AsyncAction_ExperienceReady` se spustí jednou za load experience, ne jednou za spawn.** Pro statická data. Per-life setup patří do `OnPossess`.
- **Lyra Shooting Service je sdílená a křehká.** Neupravujte ji. Bránětete ji rodičovskými rozhodnutími.
- **Observer Aborts uvnitř background větve Simple Parallelu jsou nespolehlivé.** Dejte aborting dekorátory na Selector *nad* Simple Parallel.
- **`B_CaptureTheFlagScoring.GetFlagPadByTeam` má race.** Iterujte `GetAllActorsOfClass(B_GrantFlagPad)` přímo.
- **`_FlagTeamIndex` uvnitř `B_GrantFlagPad.GrantOrDeliverFlag` je lokální proměnná.** Použijte class-level `PadTeamIndex`.
- **Listener registrovaný v `OnPossess` se duplikuje per život.** Registrujte jednou v `BeginPlay`.
- **`GetControlledPawn → GetPlayerState` uvnitř handleru eliminace může vrátit None.** Použijte vlastní `GetPlayerState` controlleru.

## Ladící postup, který vám ušetří hodiny

Když agent dělá špatnou věc, otázka je skoro vždy „čemu v tom okamžiku věřil?". Nejrychlejší cesta k odpovědi:

1. Dejte breakpoint na BT uzel, který vystřelil (pravý klik → Add Breakpoint).
2. Když PIE zastaví, otevřete **Gameplay Debugger** a snapshot blackboardu.
3. Porovnejte s tím, co *si myslíte*, že mělo být true. Najděte klíč, který nesouhlasí s realitou.
4. Trasujte zpět: co bylo poslední, co do toho klíče zapsalo? Bylo to vůbec zavoláno? Bylo to zavoláno se správnou hodnotou?

To je stejný cyklus, který byste běhali v libovolném debuggeru — jediný zvrat je, že „stav" žije na blackboardu místo v lokálních proměnných. Berte blackboard inspector jako svoje watch okno.
