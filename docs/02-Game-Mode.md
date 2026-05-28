# 02 — Game Mode

This document describes the rules of the CTF mode and the actors that participate in it. Read it before you start designing an agent — you can't write an intelligent agent for a game you don't fully understand.

## Rules

Two teams. Each team has a **flag base** at its end of the map. On each base sits a **flag**. A player or bot scores for their team by:

1. Going to the **enemy team's base**.
2. Touching the enemy flag, which **picks it up** and attaches it to the carrier.
3. Returning to their **own base** while still carrying the enemy flag.
4. **Delivering** the flag at their own base — this awards one capture point and respawns the enemy flag at its home base.

If a flag carrier is eliminated, the flag drops on the ground at their last location. Any team can interact with a dropped flag:

- Members of the flag's **own team** can return it to base on touch.
- Members of the **opposing team** can pick it up again and resume the steal.

A dropped flag will eventually auto-return to its home base if nobody interacts with it within the timeout window (configured on the flag actor / scoring component).

A match ends when a team reaches the capture-point limit, or the round timer expires (whichever the experience asset is configured for).

## The actors

### `B_GrantFlagPad` — the flag base

Located in `Content/Blueprint/B_GrantFlagPad`. Each team has one. Important members:

- **`PadTeamIndex`** *(int, class-level variable)* — which team this pad belongs to. **Use this to identify a pad's team from outside.**
- **`GetFlagTeamId()`** — accessor returning the same.
- `GrantOrDeliverFlag` — internal event handling pickup and delivery. Has a *local* variable `_FlagTeamIndex` that is **not** accessible from outside; don't try to read it.

There is also a helper `GetFlagPadByTeam` on `B_CaptureTheFlagScoring`, but it has a race-condition issue during experience load. **Don't use it.** Iterate pads directly:

```
GetAllActorsOfClass(B_GrantFlagPad) → For Each → compare PadTeamIndex
```

### `B_FlagActor` — the flag item

Located in `Content/Items/Flag/`. This is what gets carried. The pad spawns it on `BeginPlay` and re-spawns it on auto-return / delivery. Carriers parent it to a socket on their pawn.

### `B_CaptureTheFlagScoring`

Located in `Content/Blueprint/B_CaptureTheFlagScoring`. Tracks scores and broadcasts the gameplay messages listed below. Mostly fire-and-forget — your agent reads its messages, not its internals.

### Gameplay messages

The game broadcasts state changes through Lyra's **Gameplay Message Subsystem**. These are channel-based pub/sub events with a typed payload. You subscribe in Blueprint with the `Listen for Gameplay Messages` node.

| Channel                                       | Payload                | Fired when                                            |
| --------------------------------------------- | ---------------------- | ----------------------------------------------------- |
| `Lyra.CaptureTheFlag.FlagPickedUp.Message`    | `FLyraFlagStatusMessage` | A flag is picked up (from base or from ground).      |
| `Lyra.CaptureTheFlag.FlagDelivered.Message`   | `FLyraFlagStatusMessage` | A flag is delivered to a friendly base (capture).    |
| `Lyra.Elimination.Message`                    | `FLyraVerbMessage`     | Any character is eliminated (server-side only).       |

**`FLyraFlagStatusMessage`** has:

- `Instigator` — the actor that triggered the event (the picker-upper or deliverer).
- `FlagTeamId` *(int)* — the team the flag belongs to.
- `Pad` — the `B_GrantFlagPad` involved.

**`FLyraVerbMessage`** has:

- `Instigator` — the actor that dealt the killing blow.
- `Target` — **the PlayerState of the eliminated character**, not the pawn. (At the time the message fires, the pawn may already be destroyed.)
- Several other fields for verb/magnitude/source — usually unused in CTF logic.

These messages are **server-side**. They fire on the host / dedicated server. AI controllers on the server see them; clients do not. This is what you want anyway — AI lives on the server.

## Map walkthrough

`L_Limitation` is the full CTF arena. Two team bases at opposite ends, a contested middle, and several cover routes. Use this for evaluating your agent.

`L_ShooterCFT_FiringRange` is a smaller diagnostic map. Useful for testing perception and shooting without the chaos of a full match. Spawn behavior is simpler.

## Sequence of events in a typical capture

1. Red bot spawns. Its `B_ISW_AI` runs `BeginPlay → ExperienceReady`, finds both pads, sets `OwnFlagBaseLocation` (red base) and `EnemyFlagBaseLocation` (blue base) on its blackboard.
2. The behavior tree (`BT_ISW_CTF_bot`) is started by `OnPossess` and starts navigating toward `EnemyFlagBaseLocation`.
3. The red bot touches the blue flag. The Lyra Equipment / scoring system fires `Lyra.CaptureTheFlag.FlagPickedUp.Message` with `FlagTeamId = blue, Instigator = red bot`.
4. The red bot's controller listens for this message and sets `WePickedUpFlag = true` and `CarryingFlag = true` on its own blackboard.
5. Simultaneously, the **blue** bots' controllers receive the same message. They notice `FlagTeamId == own team` and set their own `OurFlagCaptured = true` and `FlagCarrier = Instigator`.
6. Blue bots switch behavior: they pursue the `FlagCarrier`. The BT uses `BTS_CheckLOS` to gate shooting on line-of-sight to the carrier.
7. Either: the red bot reaches its base and triggers `Lyra.CaptureTheFlag.FlagDelivered.Message` (score!), or it dies and `Lyra.Elimination.Message` fires with `Target = red bot's PlayerState`.
8. On elimination of the carrier, the blue controllers clear `OurFlagCaptured` and `FlagCarrier`; the red carrier's own controller clears `CarryingFlag` and `WePickedUpFlag`. The flag drops in place. See [05 — Example: Elimination Listener](05-Example-Elimination-Listener.md) for the full handler walkthrough.

This is the loop you will be tuning, replacing, or rebuilding.
