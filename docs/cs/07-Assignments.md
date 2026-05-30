# 07 — Návrhy zadání

Toto jsou návrhy semestrálních projektů. Každý je ukotven v konkrétní kapitole **Kubík, A. — *Inteligentní agenty*** (Computer Press, 2004), takže si můžete přečíst relevantní teorii dřív, než agenta začnete navrhovat.

Obtížnost: ★ snadné, ★★ střední, ★★★ těžké, ★★★★ ambiciózní.

---

## A. Chytřejší realizace existujícího rozhodování (★)

**Reference v knize:** [Kubík 2004, Kapitola 1.4 — *Jiné architektury reaktivních agentů*]; konkrétně **architektura s výběrem akce** v Kapitole 1.4.1.

**Pojmy:** Reaktivní agent s rozšířenou bází představ. V Kubíkových termínech obohacujete `I` o další představy (př. poslední známé pozice nepřátel) a měníte `akce : P × I → A` tak, aby vybraný waypoint tyto představy reflektoval.

### Varianta 1

**Co:** Baseline nositel jde z nepřátelské základny domů přímkou. Nahraďte to cestou, která se pokud možno vyhýbá místům, kde je vidět z nepřátelských pozic.

**Doporučený přístup:** Udržujte na blackboardu seznam nedávno spatřených nepřátel. Použijte **EQS (Environment Query System)** ke generování kandidátských waypointů ohodnocených podle vzdálenosti k nejbližšímu známému nepříteli. Nahraďte přímé `MoveTo(OwnFlagBaseLocation)` sekvencí volání `MoveTo(EQS-vybraný waypoint)`.

**Definice hotovo:** Přes 10 zápasů by nositel měl přežít cestu domů častěji než baseline. Změřte to.

### Varianta 2

**Co:** Baseline agent střílí a zároveň se pohybuje, což zvyšuje rozptyl zbraně. Lyra respektuje pohybový stav postavy — skrčený agent míří přesněji. Naučte agenta skrčit se, když má přímou viditelnost na nepřítele, a vstát, jakmile potřebuje běžet.

**Doporučený přístup:** Přidejte BT service nebo task k boji, který volá `GetControlledPawn → Crouch()` / `UnCrouch()` podle toho, zda agent vidí nepřítele. Pozor na to, co se stane se skrčenou polohou, když BT přejde na jiné chování.

**Definice hotovo:** V kontrolovaném testu (bot vs. bot, statická pozice, stejná vzdálenost) změřte poměr zásahů (hits / shots fired) nebo time-to-kill. Skrčený agent by měl ukázat měřitelně vyšší přesnost v přestřelkách na místě. Writeup musí diskutovat trade-off: kdy se skrčení vyplatí a kdy ne.

---

## B. Přidělování rolí v týmu (★★)

**Reference v knize:** [Kubík 2004, Kapitola 4.2 — *Koordinace*] (centralizovaná vs. decentralizovaná koordinace); a Kapitola 4.3.2 — *Kontraktační síť* (Smith 1980), pokud chcete čistě decentralizovanou formulaci.

**Co:** Teď jsou všichni boti identičtí. Všichni se snaží krást, všichni bránit. Implementujte koordinaci tak, aby se tým rozdělil: např. dva útočníci, dva obránci, jeden flex.

**Pojmy:** Multiagentové systémy. **Koordinace** mezi agenty. Můžete si vybrat:

- **Centralizovaná (přímý dozor)** — jeden koordinátor přiřazuje role. Jednoduché a předvídatelné. Mapuje se přímo na Mintzbergův *direct supervision* z Kapitoly 4.2.
- **Decentralizovaná (kontraktační síť)** — boti dávají nabídky na role, jakmile se otevřou. Blíž ke Smithovi [Kubík 2004, Kapitola 4.3.2]. Více práce, ale mnohem bohatší writeup.

**Doporučený přístup:** Přidejte server-side aktor „koordinátor týmu" (`WorldSubsystem` nebo manager aktor v experience). V `BeginPlay` controlleru se každý bot zaregistruje a dostane roli zapsanou do nového BB klíče `MyRole` (enum). Root selector BT se větví podle `MyRole`.

**Definice hotovo:** V gameplay debuggeru ověřte, že přesně existuje očekávané rozložení rolí. Ukažte, že tým s rolemi poráží tým bez rolí v head-to-head.

---

## C. Obranná pozice u podstavce vlajky (★★)

**Reference v knize:** [Kubík 2004, Kapitola 2.3 — *Mapa prostředí*] a Kapitola 2.5 — *Cílově orientovaná navigace*. Mataricin robot Toto je kanonickou referencí pro agenta, který staví prostorovou reprezentaci a podle ní pak naviguje.

**Co:** Když ještě nikdo neukradl vlajku, obránce by neměl stát na podstavci — měl by být blízko, v krytu, s výhledy na nájezdové cesty.

**Pojmy:** Prostorové uvažování. Reaktivní agent s lokálně optimalizovaným umístěním. Kde Toto měl mapu jako graf značek `(LW, RW, C, J, …)`, váš agent použije UE5 NavMesh + EQS jako ekvivalentní prostorovou reprezentaci.

**Doporučený přístup:** Sestavte EQS dotaz, který ohodnocuje pozice podle: vzdálenosti k vlastnímu podstavci vlajky (v rozsahu), krytí před hlavními přístupovými vektory, a přímé viditelnosti na přístup. Pravidelně spouštějte dotaz ve službě a nejlepší pozici zapisujte do nového BB klíče `DefensePost`. Použijte ho jako `MoveGoal` v default větvi, když je agent v roli „defender".

**Definice hotovo:** S lidským útočníkem by obránce měl znatelně ztížit dosažení vlajky oproti baseline.

---

## D. Deliberativní agent řízený cíli (★★★)

**Reference v knize:** [Kubík 2004, Kapitola 2 — *Uvažující agent*], obzvlášť Kapitoly 2.7–2.9 (teorie BDI: **Představa – Touha – Záměr**). Volitelně Kapitola 2.10 — architektura IRMA jako implementační reference.

**Co:** Nahraďte root selector BT mechanismem výběru cíle, který vybírá z množiny explicitních cílů — např. `StealFlag`, `RecoverFlag`, `ProtectBase`, `KillCarrier`, `Restock` — tak, že každý ohodnotí proti aktuálním představám. Pak dispatch do sub-BT pro vybraný cíl.

**Pojmy:** Deliberativní (uvažující) agent. Explicitní cíle (`Touha` / `Cíl` v BDI), explicitní záměry (`Záměr`). Utility-based decision making. Toto je nejčistší zapadnutí pro BDI-inspirovaného controllera v knižní typologii.

**Doporučený přístup:** Nechte existující podstromy (recover, return, steal) ale nahraďte top-level řetěz dekorátorů vlastním BT taskem `BTT_SelectGoal`, který spustí vaši scoring funkci, výsledek zapíše do nového BB klíče `CurrentGoal` a skončí. Vrstva nad ním je switch, který vybírá podstrom podle `CurrentGoal`. Scoring funkce je váš výběr `záměru`.

**Definice hotovo:** Cíle by se měly měnit *plynule* během hry a agent by měl každou změnu zdůvodnit jasným rozdílem skóre. Přidejte debug print „vybraný cíl: X protože skóre = …", abyste si mohli číst rozhodovací stopu.

---

## F. Úplné nahrazení BT jinou architekturou (★★★★)

**Reference v knize:** [Kubík 2004, Kapitola 3.2 — *Hybridní agent*]; konkrétně architektura **InteRRaP** (Müller 1996) z Kapitoly 3.2.1, s jejími vrstvami reaktivního / lokálního plánovacího / kooperativního plánovacího chování a jejich `řídicími cykly`.

**Co:** Postavte controller od nuly (podědíte `LyraPlayerBotController`), který používá jinou rozhodovací architekturu — vrstvený hybrid v duchu InteRRaP, konečný automat, nebo malý GOAP planner. Spusťte ho head-to-head proti BT-based baseline.

**Pojmy:** Přímé srovnání architektur agentů z [Kubík 2004]. InteRRaP-style vrstvené řízení je přirozeným cílem, protože každá z jeho tří vrstev (reaktivní, lokální plánování, kooperativní plánování) odpovídá jinému druhu rozhodnutí, který v projektu už máte — přestřelky jsou reaktivní, návrat domů je lokální plánování, koordinace týmu z projektu B je kooperativní plánování.

**Doporučený přístup:** Existující blackboard nechte jako bázi představ (jeho tvar je rozumný pro `I` nezávisle na architektuře). Nahraďte BT vlastní rozhodovací smyčkou na `Tick` nebo vlastním subsystémem. Listenery herních zpráv znovu použijte přes malý interface mixin, pokud preferujete modularitu.

**Definice hotovo:** Přímé srovnávací zápasy (vaše architektura vs. BT baseline) přes alespoň 20 kol, s writeupem, kde každá architektura vyhrávala a kde prohrávala. Srovnání architektur je dodatelný výstup; „vítězství" se nevyžaduje.

---

## Hlášení a deliverables

Pro libovolnou ze zadání očekávejte odevzdat:

1. **Implementaci** v `Content/Bot/Student_<jmeno>/` — child Blueprint od `B_ISW_AI` plus vlastní Behavior Tree a podpůrné assety.
2. **Krátký writeup** (pár stran) popisující návrh agenta **v Kubíkově terminologii** — jasně identifikujte, ke kterým sekcím knihy se vaše architektura vztahuje, kde se odchyluje a proč.
3. **Záznam dema** agenta v akci proti baseline.

Pokud zadání mění experience nebo přidává aktory, dokumentujte ty změny ve writeupu, aby budoucí studenti mohli číst váš kód bez špelunkingu.

## Co hledáme

Přibližně v sestupném pořadí důležitosti:

1. **Korektnost.** Váš agent dělá to, co váš návrh říká, že má dělat — ověřitelné v gameplay debuggeru.
2. **Zdůvodněný návrh.** Vaše volby se čistě mapují na pojmy z Kubíka. Můžete vysvětlit *proč* jste vybrali tu architekturu, s citací příslušných sekcí, ne jenom *co* dělá.
3. **Důkaz.** Nějaká kvantitativní evaluace, byť neformální. „Vyhraje 7/10 zápasů" je lepší než „cítí se silnější".
4. **Hygiena kódu.** Držte se vzorů z [06 — Rozšiřování AI](06-Extending-the-AI.md). Dědíte od `B_ISW_AI` — sdílené baseline assety přímo neupravujte.
5. **Polish.** Stretch goals jako hezčí chování, animace nebo koordinovaná týmová hra se cení, ale nejsou nutné.

Přečtěte si alespoň jeden projekt spolužáka na konci semestru — porovnávat různé architektury agentů na stejné hře je součástí učení [Kubík 2004, Kapitola 3.2].
