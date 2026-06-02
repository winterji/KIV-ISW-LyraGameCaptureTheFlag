# 02 — Game Mode

This document describes the rules of the CTF mode and the actors that participate in it. Read it before you start designing an agent — you can't write an intelligent agent for a game you don't fully understand.

## Rules

Two teams. Each team has a **flag base** at its end of the map. On each base sits a **flag**. A player or bot scores for their team by:

1. Going to the **enemy team's base**.
2. Touching the enemy flag, which **picks it up** and attaches it to the carrier.
3. Returning to their **own base** while still carrying the enemy flag.
4. **Delivering** the flag at their own base — this awards one capture point and respawns the enemy flag at its home base.

If a flag carrier is eliminated, the flag returns to its base. There is not just one flag. If a team steals one flag, it can steal the other as well.

A match ends when a team reaches the capture-point limit, or the round timer expires (according to the experience configuration).

## The actors

### `B_GrantFlagPad` — the flag base

Located in `Content/Blueprint/B_GrantFlagPad`. Each team has one. Important members:

- **`PadTeamIndex`** *(int, class-level variable)* — which team this pad belongs to. Use this to get the Team ID of a given flag pad.
- **`GetFlagTeamId()`** — accessor returning the same.
- `GrantOrDeliverFlag` — internal event handling pickup and delivery.

There is also a helper `GetFlagPadByTeam` on `B_CaptureTheFlagScoring`, but it has a race-condition issue during experience load. **Don't use it.** Iterate pads directly:

```
GetAllActorsOfClass(B_GrantFlagPad) → For Each → compare PadTeamIndex
```

![get_flagpad_team_index](./img/get_flagpad_team_index.png)

### `B_FlagActor` — the flag item

Located in `Content/Items/Flag/`. This is what gets carried. The pad spawns it on `BeginPlay` and re-spawns it on auto-return / delivery. Carriers parent it to a socket on their pawn.

### `B_CaptureTheFlagScoring`

Located in `Content/Blueprint/B_CaptureTheFlagScoring`. Tracks scores and broadcasts the gameplay messages listed below.

### Gameplay messages

The game broadcasts state changes through Lyra's **Gameplay Message Subsystem**. These are pub/sub events on named channels with a typed payload. You subscribe in Blueprint with the `Listen for Gameplay Messages` node.

In Kubík's terminology this mechanism is close to **komunikační akty** — explicit messages in a higher-level language independent of the agent's physical visibility — with elements of **reaktivní komunikace / stigmergie**, because the emitter is not the agents themselves but the game system updating a "trace" in the environment.

| Channel                                       | Payload type           | Fired when                                            |
| --------------------------------------------- | ---------------------- | ----------------------------------------------------- |
| `Lyra.CaptureTheFlag.FlagPickedUp.Message`    | `LyraFlagStatusMessage` | A flag is picked up (from base or from ground).      |
| `Lyra.CaptureTheFlag.FlagDelivered.Message`   | `LyraFlagStatusMessage` | A flag is delivered to a friendly base (capture).    |
| `Lyra.Elimination.Message`                    | `LyraVerbMessage`      | Any character is eliminated (server-side only).       |

**`LyraFlagStatusMessage`** has:

- `Instigator` — the actor that triggered the event (the picker-upper or deliverer).
- `FlagTeamId` *(int)* — the team that the agent who picked up the flag belongs to - `Instigator`.
- `Pad` — the `B_GrantFlagPad` the flag came from.

**`LyraVerbMessage`** has:

- `Instigator` — the actor that dealt the killing blow.
- `Target` — **the PlayerState of the eliminated character**, not the pawn. (At the time the message fires, the pawn may already be destroyed.)
- Several other fields (verb/magnitude/source) — you likely won't need them in CTF logic.

Messages are broadcast **server-side**. They fire on the host / dedicated server. AI controllers on the server see them; clients do not. This suits us — AI lives on the server.

**Tips:**
- Before using data from a received message, you first need to "break" the message into individual key-value pairs using `Break <Payload type>`.
- `Target` in `LyraVerbMessage` is of type `PlayerState`, and when checking it against another agent you must take their `PlayerState` (see screenshot below).

![break_message](./img/break_message.png)

## Map walkthrough

`L_Limitation` is the full CTF arena. Two team bases at opposite ends, a contested middle, and several cover routes. Use this for evaluating your agent.

`L_ShooterCFT_FiringRange` is a smaller diagnostic map. Useful for testing perception and shooting without the chaos of a full match. Spawn behavior is simpler.

## Sequence of events in a typical capture

1. Red bot spawns. Its `B_ISW_AI` runs `BeginPlay → ExperienceReady`, finds both pads, sets `OwnFlagBaseLocation` (red base) and `EnemyFlagBaseLocation` (blue base) on its blackboard.
2. The behavior tree (`BT_ISW_CTF_bot`) is started by `OnPossess` and starts navigating toward `EnemyFlagBaseLocation`.
3. The red bot touches the blue flag. The Lyra Equipment / scoring system fires `Lyra.CaptureTheFlag.FlagPickedUp.Message` with `FlagTeamId = blue, Instigator = red bot`.
4. The red bot's controller listens for this message and sets `WePickedUpFlag = true` and `CarryingFlag = true` on its own blackboard.
5. Simultaneously, the **blue** bots' controllers receive the same message. They notice `FlagTeamId == their team` and set their own `OurFlagCaptured = true` and `EnemyFlagCarrier = Instigator`.
6. Blue bots switch behavior: they pursue the `EnemyFlagCarrier`. The BT uses `BTS_CheckLOS` to gate shooting on line-of-sight to the carrier.
7. Either: the red bot reaches its base and triggers `Lyra.CaptureTheFlag.FlagDelivered.Message` (score!), or it dies and `Lyra.Elimination.Message` fires with `Target = red bot's PlayerState`.
8. On elimination of the carrier, the blue controllers clear `OurFlagCaptured` and `EnemyFlagCarrier`; the red carrier's own controller clears `CarryingFlag` and `WePickedUpFlag`. The flag drops in place. See [05 — Example: flag pickup listener](05-Example-Elimination-Listener.md) for the full handler walkthrough.

This is the loop you will be tuning, replacing, or rebuilding.
