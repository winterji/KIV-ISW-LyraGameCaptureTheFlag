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

## Nahrazení existujícího controlleru

Pokud vaše zadání vyžaduje jinou architekturu agenta — BDI-style deliberativního agenta [Kubík 2004, §2.9], InteRRaP-style hybrida [Kubík 2004, §3.2.1], utility-based selector, atd. — máte dvě možnosti:

**Možnost A — Podědit `B_ISW_AI`.** Zděďte, override `OnPossess`, životní cyklus a listenery nechte být. Použijte, když chcete znovu použít zapojení percepce a zpráv a měníte jen rozhodovací logiku.

**Možnost B — Podědit `LyraPlayerBotController` přímo.** Začněte na zelené louce. Použijte, když chcete úplně odstranit BT a běhat vlastní rozhodovací smyčku na `Tick`.

U možnosti B nezapomeňte:

- Ručně replikovat rozdělení fází životního cyklu: ExperienceReady pro statický setup, `OnPossess` pro per-life setup.
- Ručně registrovat listenery herních zpráv ve správné fázi.
- Nastavit `Default Pawn Class` v experience na Lyra postavu s vybavením, které potřebujete (zbraně, komponentu pro pickup vlajky).

Tak či onak, experience asset (`B_ShooterGame_CaptureTheFlag` nebo váš fork) musí ukazovat na vašeho nového controllera. Úprava experience je jediný způsob, jak skutečně controller vyměnit — nastavení na Default Pawn bota nepomůže, protože Lyra spawnuje boty skrz experience.

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
