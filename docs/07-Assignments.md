# 07 — Suggested Assignments

These are project ideas for your semestral work. Each one is anchored to a specific chapter of **Kubík, A. — *Inteligentní agenty*** (Computer Press, 2004) so you can read the relevant theory before designing your agent.

Difficulty: ★ easy, ★★ moderate, ★★★ hard, ★★★★ ambitious.

---

## A. Smarter return path for the flag carrier (★)

**Book reference:** [Kubík 2004, Chapter 1.4 — *Jiné architektury reaktivních agentů*]; specifically the **architektura s výběrem akce** in Chapter 1.4.1.

**What:** The baseline carrier walks in a straight line from the enemy base to its own base. Replace this with a path that avoids being seen by enemy positions where possible.

**Concepts:** Reactive agent with an extended belief base. In Kubík's terms, you are enriching `I` with additional beliefs (last-seen enemy positions) and changing `akce : P × I → A` so that the chosen waypoint reflects those beliefs.

**Suggested approach:** Maintain a blackboard list of recently-seen enemies. Use **EQS (Environment Query System)** to generate candidate waypoints scored by distance to nearest known enemy. Replace the straight `MoveTo(OwnFlagBaseLocation)` with a sequence of `MoveTo(EQS-chosen waypoint)` calls.

**Definition of done:** Over 10 matches, the carrier should survive home more often than the baseline does. Measure it.

---

## B. Role assignment across the team (★★)

**Book reference:** [Kubík 2004, Chapter 4.2 — *Koordinace*] (centralized vs. decentralized coordination); and Chapter 4.3.2 — *Kontraktační síť* / contract net protocol (Smith 1980) if you want a clean decentralized formulation.

**What:** Right now, every bot is identical. They all try to steal, they all defend. Implement a coordination mechanism so the team distributes itself: e.g., two attackers, two defenders, one flex.

**Concepts:** Multi-agent systems. **Koordinace** between agents. You may pick:

- **Centralized (přímý dozor)** — a single coordinator actor assigns roles. Simple and predictable. Maps directly onto Mintzberg's *direct supervision* discussed in Chapter 4.2.
- **Decentralized (kontraktační síť)** — bots bid for roles when openings appear. Closer to Smith's contract net [Kubík 2004, Chapter 4.3.2]. More work but a much richer write-up.

**Suggested approach:** Add a server-side "team coordinator" actor (a `WorldSubsystem` or a manager actor in the experience). On controller `BeginPlay`, each bot registers and gets a role written to a new BB key `MyRole` (enum). The BT's root selector branches on `MyRole`.

**Definition of done:** Confirm via the gameplay debugger that exactly the expected distribution of roles is present. Show that role-aware teams beat role-flat teams in head-to-head.

---

## C. Ammo and resource awareness (★★)

**Book reference:** [Kubík 2004, Chapter 1.2 — *Čistě reaktivní agent*] for the formal model; the `OutOfAmmo` flag is one more dimension of the internal state set `I`.

**What:** The `OutOfAmmo` BB key already exists but is unused. Make the agent stop firing when low on ammo, retreat to a safer position, and re-engage when it has restocked (Lyra has weapon pickups on most maps).

**Concepts:** Internal state extending beyond positional belief. Goal selection driven by resource constraints. In a reactive setting this is still pure `akce : P × I → A`; in a deliberative setting (project E below) you would re-rank goals when ammo runs out.

**Suggested approach:** Subscribe to Lyra's weapon ammo events (look in `LyraEquipmentManagerComponent` for the broadcast). Set `OutOfAmmo` from that. Add a new top-level BT branch with a higher decorator priority: `[OutOfAmmo == true] → MoveTo(nearest weapon pickup)`.

**Definition of done:** A bot mid-firefight that runs out of ammo visibly disengages, restocks, and returns — and survives more engagements than the baseline.

---

## D. Defensive positioning around a flag pad (★★)

**Book reference:** [Kubík 2004, Chapter 2.3 — *Mapa prostředí*] and Chapter 2.5 — *Cílově orientovaná navigace*. Mataric's Robot Toto is the canonical reference for an agent that builds a spatial representation it then navigates against.

**What:** When no flag has been stolen yet, a defender should not be standing on the flag pad — it should be near it, in cover, with sight lines on approaches.

**Concepts:** Spatial reasoning. Reactive agent with locally optimized positioning. Where Toto's map was a graph of markers `(LW, RW, C, J, …)`, your agent will use UE5's NavMesh + EQS as the equivalent spatial representation.

**Suggested approach:** Build an EQS query that scores positions by: distance to own flag pad (within range), cover from major approach vectors, and line-of-sight to the approach. Run the query periodically in a service and write the best location to a new BB key `DefensePost`. Use it as `MoveGoal` in the default branch when the agent is in the "defender" role.

**Definition of done:** With a human attacker, the defender should make it noticeably harder to reach the flag than in the baseline.

---

## E. Goal-driven deliberative agent (★★★)

**Book reference:** [Kubík 2004, Chapter 2 — *Uvažující agent*], especially Chapters 2.7–2.9 (BDI theory: **Představa – Touha – Záměr**). Optionally Chapter 2.10 — IRMA architecture for an implementation reference.

**What:** Replace the BT's root selector with a goal-selection mechanism that picks from a set of explicit goals — e.g. `StealFlag`, `RecoverFlag`, `ProtectBase`, `KillCarrier`, `Restock` — by scoring each one against current beliefs. Then dispatch into a sub-BT for the chosen goal.

**Concepts:** Deliberative agent. Explicit goals (`Touha`/`Cíl` in BDI), explicit intentions (`Záměr`). Utility-based decision making. This is the cleanest fit for a BDI-inspired controller in the book's typology.

**Suggested approach:** Keep the existing sub-trees (recover, return, steal) but replace the top-level decorator chain with a custom BT Task `BTT_SelectGoal` that runs your scoring function, writes the result to a new `CurrentGoal` BB key, and finishes. The level above it is a switch that picks the sub-tree by `CurrentGoal`. The scoring function is your `záměr` selection.

**Definition of done:** Goals should change *fluidly* during play, and the agent should justify each switch by a clear scoring difference. Add a debug print of "selected goal: X because score = ..." so you can read the decision trail.

---

## F. Modeling other agents — predicting the carrier's path (★★★)

**Book reference:** [Kubík 2004, Chapter 2.8 — *Teorie intencionálních systémů*]. Dennett's **intencionální postoj** (intentional stance) — attributing beliefs and goals to another agent in order to predict its behavior.

**What:** When our flag is stolen, the recovery branch chases the carrier reactively. Build an agent that *predicts* where the carrier is going (almost always: the enemy base, via likely routes) and ambushes them instead.

**Concepts:** Modeling another agent's mental states (in the sense of Chapter 2.8). You are treating the enemy carrier as an *intentional system* with a known goal and inferring their likely actions from it.

**Suggested approach:** Treat the carrier as a known agent with a known goal (their own base). Generate likely paths (via UE's NavMesh A*, or sample several plausible routes via EQS). Compute an intercept waypoint, write it to `MoveGoal`, and route there instead of `MoveTo(FlagCarrier)`.

**Definition of done:** When the human-controlled carrier is intentionally taking a less-direct route, the predictive agent should sometimes guess wrong — that's good. The point is to be *better on average*, not always right.

---

## G. Replace the BT entirely with a different architecture (★★★★)

**Book reference:** [Kubík 2004, Chapter 3.2 — *Hybridní agent*]; specifically the **InteRRaP** architecture (Müller 1996) discussed in Chapter 3.2.1, with its reactive / local-planning / cooperative-planning layers and their `řídicí cykly`.

**What:** Build a controller from scratch (subclassing `LyraPlayerBotController`) that uses a different decision-making architecture — a layered hybrid in the spirit of InteRRaP, a finite-state machine, or a small GOAP planner. Run it head-to-head with the BT-based baseline.

**Concepts:** Direct comparison of agent architectures from [Kubík 2004]. InteRRaP-style layered control is a natural target because each of its three layers (reactive, local-planning, cooperative-planning) maps to a distinct kind of decision you already have in the project — gunfights are reactive, returning home is local planning, team coordination from project B is cooperative planning.

**Suggested approach:** Keep using the existing blackboard as the belief base (it's a perfectly reasonable shape for `I` regardless of architecture). Replace the BT entirely with your own decision loop on `Tick` or a custom subsystem. Reuse the gameplay-message listeners by parking them on a small interface mixin if you prefer modularity.

**Definition of done:** A direct comparison match (your architecture vs. the BT-based baseline) over at least 20 rounds, with a write-up of where each architecture won and lost. The architecture comparison is the deliverable; "winning" is not required.

---

## Reporting and deliverables

For any of these, expect to deliver:

1. **The implementation** in `Content/Bot/Student_<yourname>/` — a child Blueprint of `B_ISW_AI` plus your own Behavior Tree and any supporting assets.
2. **A short writeup** (a few pages) describing the agent design **in Kubík's terminology** — clearly identify which sections of the book your architecture corresponds to, where it deviates, and why.
3. **A demo recording** of the agent in action against the baseline.

If your assignment changes the experience or adds new actors, document those changes in your writeup so future students can read your code without spelunking.

## What we're looking for

In approximately decreasing order of importance:

1. **Correctness.** Your agent does what your design says it should do — verifiable in the gameplay debugger.
2. **Justified design.** Your choices map cleanly onto agent concepts from Kubík. You can explain *why* you chose the architecture you did, citing the relevant sections, not just *what* it does.
3. **Evidence.** Some quantitative evaluation, however informal. "Wins 7/10 matches" beats "feels stronger".
4. **Code hygiene.** Follow the patterns in [06 — Extending the AI](06-Extending-the-AI.md). Inherit from `B_ISW_AI` — do not edit the shared baseline assets directly.
5. **Polish.** Stretch goals like nicer behavior, animation, or coordinated team play are appreciated but not required.

Read at least one classmate's project at the end of term — comparing different agent architectures on the same game is part of the learning [Kubík 2004, Chapter 3.2].
