# 03 — AI Architecture

This document maps the concepts of **Kubík, A. — *Inteligentní agenty*** onto the concrete Lyra / Unreal AI stack you will work with. If you understand both sides of the mapping, the rest of the documentation will read naturally.

## 3.1 Kubík's definition of an agent

The starting point of the book is the definition on page 12:

> *„Agent je entita zkonstruovaná za účelem kontinuálně a do jisté míry autonomně plnit své cíle v adekvátním prostředí na základě vnímání prostředí prostřednictvím **senzorů** a prováděním akcí prostřednictvím **aktuátorů**. Agent přitom ovlivňuje podmínky v prostředí tak, aby se přibližoval k plnění cílů."*
> — [Kubík 2004, Úvod, s. 12]

Kubík places three properties above all others:

- **autonomie** (autonomy),
- **existence v prostředí** (existence in an environment), and
- **kontinuita senzorického a aktuačního propojení s prostředím** (continuous sensor/actuator coupling with the environment).

Everything else — perception, beliefs, decisions, actions — sits inside that frame.

## 3.2 Formal model

The book gives the formal model of a reactive agent as a 6-tuple:

```
{ P, A, I, vjem, změna_stavu, akce }
```

where

- `P` — set of possible **percepts** (vjemy)
- `A` — set of possible **actions** (akce)
- `I` — set of possible **internal states** (vnitřní stavy)
- `vjem : E → P` — perception function: environment state → percept
- `změna_stavu : P × I → I` — state-update function
- `akce : P × I → A` — action-selection function

A subset `C ⊆ I` is the set of **goal states** (cíle). A purely reactive agent drops `I` entirely and reduces to `akce : P → A`.

Our agent in code has exactly this shape:

| Symbol           | In ShooterCTF                                                              |
| ---------------- | -------------------------------------------------------------------------- |
| `E`              | The Unreal world: pawns, geometry, items, flags, flag pads.                |
| `vjem`           | `AIPerceptionComponent` + gameplay-message listeners.                      |
| `P`              | The instantaneous information delivered by perception (a perceived enemy, a "flag picked up" message). |
| `I`              | The Blackboard (`BB_ISW_Bot`).                                             |
| `změna_stavu`    | Whatever writes to the Blackboard — controller event handlers, BT services, decorators. |
| `akce`           | The Behavior Tree's decision of which leaf task to run next.               |
| `A`              | BT Tasks: `MoveTo`, the Lyra shooting service, custom tasks you write.     |
| `C`              | Implicit. Our reactive baseline has no explicit `C`. Plays to the end.     |

## 3.3 The four architecture types

The book divides agents into four architecture classes. Locating your work on this map is the first thing to do when designing an agent:

| Architecture      | Book reference    | Defining property                                                          | Game example                                                     |
| ----------------- | ----------------- | -------------------------------------------------------------------------- | ---------------------------------------------------------------- |
| **Reaktivní**     | Chapter 1                | No internal model of the world. Action is a function of current percept (and maybe a small buffer). | A Quake bot with reflex shooting and a small state machine.    |
| **Deliberativní (uvažující)** | Chapter 2 | Symbolic representations of the world; plans toward goals. Possibly combined with BDI theory. | An RTS bot that pathfinds, books production queues, ranks targets by utility. |
| **Sociální**      | Chapters 3.1 and 4 | Communicates with other agents in a higher-level language.               | Coordinated CTF teams exchanging role assignments.              |
| **Hybridní**      | Chapter 3.2  | Layered combination of the above.                                           | The shipped `B_ISW_AI` is closest here — reactive at the leaves, with a symbolic belief base. |

### 3.3.1 Where the baseline sits

The baseline `B_ISW_AI`:

- Maintains a **symbolic belief base** (Blackboard) — pulls it toward *deliberativní*.
- Selects actions reactively from current beliefs — pulls it back toward *reaktivní*.
- Does not currently communicate with other agents or model their beliefs.

In spirit it is close to the *uvažující agent na bázi reaktivity* / Robot Toto from Chapter 2.1: reactive execution layered on top of a symbolic representation of the world.

### 3.3.2 Is the baseline BDI?

It's tempting to read the BT through a BDI lens — and the reading is largely valid. The baseline has all three BDI categories present:

- **Beliefs (Představy)** — **explicitly** in the Blackboard. Every BB key is a belief about the world.
- **Desires / Goals (Touhy / Cíle)** — **implicitly** in the decorator preconditions on each subtree. `[OurFlagCaptured == true]` reads as `Goal(self, FlagRecovered)`; `[CarryingFlag == true]` reads as `Goal(self, FlagDelivered)`. The Observer Aborts machinery activates the goal whose precondition has just become true.
- **Intentions (Záměry)** — **implicitly** as the currently-active subtree path. The same Observer Aborts mechanism is functionally Bratman's intention-revision: a commitment that gets retracted the moment its activation condition fails.

So in the loose sense common in game AI, the baseline **is BDI-inspired**.

It diverges from Kubík's strict BDI architecture [Kubík 2004, Chapters 2.8.3, 2.9, 2.10] in three concrete ways:

1. **No first-class data for desires and intentions.** In IRMA ([Kubík 2004, Obr. 2.8, s. 56]) beliefs, desires, intentions, and plans are *four separately stored knowledge bases* that the deliberation cycle reads from and writes to. In the baseline, desires and intentions live in BT topology — visible in the editor and the Gameplay Debugger, but not queryable as runtime data, not modifiable from outside, and not transferable to another agent.
2. **No deliberation cycle.** The canonical BDI loop ([Kubík 2004, p. 54]) is `events ← percept() → bel ← belrev() → goal ← options() → int ← filter() → pl ← plan() → execute()`. The BT does `perceive → walk-tree-in-priority → execute`. The pieces missing are explicit option enumeration (`options()`) and the IRMA filtering machinery — `filter kompatibility`, `analyzátor příležitostí`, `filter override mechanism` (all Chapter 2.10).
3. **No plan library or plan generation.** Subtrees are hardcoded execution policies, not a `knihovna plánů` indexed by goal that the agent can search.

## 3.4 Mapping to UE5 / Lyra concepts

| Kubík concept                                    | Where it lives in Lyra / Unreal                                                                                                |
| ------------------------------------------------ | ----------------------------------------------------------------------------------------------------------------------------- |
| **Tělo / aktuátory**                              | The `Character` Blueprint controlled by the AI — mesh, animations, physics, weapon.                                          |
| **Senzory**                                       | `AIPerceptionComponent` (sight, hearing, damage) on the AI Controller. Custom BT services that poll the world also count.    |
| **Vnitřní stav `I`**                              | The **Blackboard** (`BB_ISW_Bot`).                                                                                            |
| **Funkce vjemu (`vjem`)**                         | `AIPerception` + gameplay-message handlers generated server-side.                                                            |
| **Funkce změny stavu (`změna_stavu`)**            | BT Services (e.g. `BTS_CheckLOS`) and message handlers — anything that derives beliefs from raw percepts.                    |
| **Funkce výběru akce (`akce`)**                   | The **Behavior Tree** (`BT_ISW_CTF_bot`). Each tick it walks the tree and chooses the active leaf.                           |
| **Akční potenciál (`A`)**                         | BT Tasks: `MoveTo`, the Lyra shooting service, anything you implement.                                                       |
| **Mozek agenta**                                  | The **AI Controller** (`B_ISW_AI`) — owns perception, blackboard, behavior tree, and event handling.                         |

A few things worth flagging:

- **The AI Controller outlives the pawn**. When the pawn dies, the controller stays. On respawn the same controller `OnPossess`es a new pawn. So anything that should persist across deaths (gameplay-message listeners) goes on the controller, not on the pawn.
- **The Blackboard is not a key-value scratchpad** — in Kubík's terms it's `I`, the agent's internal state, and it should be treated as a belief base. Add a key only when it represents a meaningful proposition about the world; write to it only when that proposition actually changes.

## 3.5 Subsumption architecture and the BT

Brooks's **subsumpční architektura** describes what a Behavior Tree does in practice. Brooks's key properties:

- **Tělesnost** (embodiment) — the agent has a body and sensors/actuators that exist in the world. The agent has a body — the pawn.
- **Situovanost** (situatedness) — the agent is embedded in a real environment with which it interacts continuously. The agent is on the map and perceives its surroundings via AIPerception.
- **Inteligence** — emerges from the interaction between simple modules and the world. BT decorators / subtrees are analogous to Brooks's competence layers.
- **Emergence** — system behavior arises from interactions between components, not from a central plan. A bot that goes to defend the base when the flag is stolen is an example: nowhere in the code is "defender role" written; it is a consequence of decorator state and the LOS service.

The mechanism Brooks called **potlačení a zabránění** (suppression and inhibition) between layers is exactly what BT decorators with *Observer Aborts: Lower Priority* do in our tree: a higher-priority subtree (recover flag) suppresses lower ones (steal) when its activation condition becomes true. In Brooks's robots this was wired with explicit signal taps; in a BT it is wired with a decorator and Observer Aborts.

The baseline agent does not use a deliberative BDI architecture, but is only BDI-inspired and behaves reactively with an internal symbolic representation of the world.

## 3.6 Controller lifecycle

The trickiest part of the engine. Get it wrong and your agent stops thinking after its first death. Get it right and most other things become tractable.

Three relevant phases:

### Phase A — `BeginPlay` and Experience-ready

`BeginPlay` fires once on the controller, very early. Lyra additionally has an asynchronous **Experience** loading step — game features (like ShooterCTF) are not necessarily registered yet on `BeginPlay`. To wait for them, use the node `AsyncAction_ExperienceReady → OnReady`. When you inherit from **`B_ISW_AI`** and connect `Parent BeginPlay` directly after `Event BeginPlay`, it will wait for ExperienceReady and you can build on top of that.

![begin_play](./img/begin_play.png)

Use this phase for registering gameplay-message listeners that should survive respawns.

**Do not** use this phase for:

- Starting the behavior tree (must be re-run on each possession, see Phase B).
- Anything that depends on the pawn existing — the pawn may not exist yet.

### Phase B — `OnPossess`

Fires every time the controller takes possession of a pawn — at initial spawn and after every respawn.

Use this phase for:

- Calling `UseBlackboard(BB_ISW_Bot)` and **caching its output pin** into a controller variable (commonly called `BBComp`).
- Calling `RunBehaviorTree(BT_ISW_CTF_bot)`.
- Resetting **per-life** blackboard keys (`CarryingFlag`, `WePickedUpFlag`, etc.).

**Why caching matters.** If you call `GetBlackboard()` on the controller before `RunBehaviorTree` has actually started the tree, it returns `None`. Code that calls `Self → GetBlackboard → SetValueAsBool` will silently no-op. The robust pattern is:

> Drag a wire from `UseBlackboard`'s return pin, promote to variable, name it `BBComp`. Use `BBComp → SetValueAsBool(...)` everywhere thereafter. Guard with `IsValid(BBComp)`.

<!-- AI generated, not tested

### Phase C — Runtime events

Gameplay messages, perception events, anim notifies, etc. run on the controller because that's where you registered them in Phase A. Inside the handler:

- Read the AI's identity via the **controller's own** `GetPlayerState`, not via `GetControlledPawn → GetPlayerState`. The pawn may already be `None`.
- Use cached `BBComp` for all blackboard reads/writes.
- Keep handlers small. Update the **belief base** (the blackboard `I`); let the BT pick up the change on its next tick. **Do not call `MoveTo` or trigger shooting from a message handler.** This corresponds exactly to Kubík's separation of `změna_stavu` (handlers update `I`) from `akce` (the tree decides what to do). -->

## 3.7 Three flavours of perception

Three kinds of "perception" coexist in this project — and they all collapse into the single `vjem` function in Kubík's formal model. In practice you should use them intentionally:

1. **Lyra `AIPerceptionComponent`** — sight and hearing cones configured on the controller. Fires `OnTargetPerceptionUpdated`. This is what populates `TargetEnemy`.
2. **Behavior-tree services** — small Blueprint scripts that tick while a subtree is active. `BTS_CheckLOS` polls `LineOfSightTo(FlagCarrier)` and writes the boolean result. Analogous to Brooks's *kompetenční moduly*.
3. **Gameplay-message subscriptions** — pub/sub events broadcast by the game (`Lyra.CaptureTheFlag.FlagPickedUp.Message`, …). These are "perceptual" in the sense Kubík uses in Chapter 4.2.1 when discussing **reaktivní komunikace** / **stigmergie**: agents read traces in the environment instead of directly observing each other. Gameplay messages are a slightly higher-level form of the same idea: explicit broadcast rather than implicit trace.

Each has different cost, fidelity, and timing. There is no canonical answer to which one you should use — design the blend that fits the behavior you want.

## 3.8 What a "thinking step" looks like in code

A single tick of the agent:

1. The `AIPerceptionComponent` may fire `OnTargetPerceptionUpdated`, updating `TargetEnemy` on the blackboard. (`vjem → změna_stavu`)
2. The `BT_ISW_CTF_bot` ticks. Decorators evaluate against current blackboard state. (read `I`)
3. The currently-active subtree's services (e.g. `BTS_CheckLOS`) run their tick logic and update keys. (`změna_stavu` derived from new `vjem`)
4. The selected leaf task runs (e.g. `MoveTo`, the Lyra shooting service, a custom task you wrote). (`akce`)
5. Asynchronously, gameplay messages may fire and update the blackboard out-of-band. (`vjem → změna_stavu`)

That's it. The loop is short, simple, and reactive. The interesting work is in deciding *what beliefs* to maintain, *how to update them*, and *how to structure the tree* so that the right behavior emerges from those beliefs. In Kubík's language: you are designing `I`, the two state-update/action functions, and optionally `C`.
