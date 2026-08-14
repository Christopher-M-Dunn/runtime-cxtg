# Basis Spec Comparison

Side-by-side of the current CX TG draft vs. the SoftCPU SIG's 2022 CX draft ("the basis spec"), for design-lineage context only.

**Scope rule:** the basis spec is historical only. It never defines or modifies a `Composable-Extensions.md` requirement. If it fills a gap the current spec leaves open, it may inform a tentative assumption in that document's `## Discussion` section — labeled **basis spec (historical, non-normative)** — never a `Requirements` section. Current TG spec always wins on conflict.

## Sources

| | Current TG draft | Basis spec |
|---|---|---|
| Title | RISC-V Composable Custom Extensions | Draft Proposed RISC-V CX Specification |
| Version | v0.0.0, draft dated 2026-07-24 | v0.95.240403 (2024-04-03) |
| Authors | Darius Rad, Jan Gray | SoftCPU SIG |
| Local copy | `../composable-custom-extensions/build/composable-custom-extensions.pdf` | `Basis_and_old_stuff/CXTG - Draft Spec Version 0.95.240403 2024-04-03 Draft.pdf` (shared drive; byte-identical to origin, confirmed 2026-07-31) |
| Origin | github.com/riscv/composable-custom-extensions | raw.githubusercontent.com/grayresearch/CX (branch `28-cx-state-context-csrs`) |

---

## §2.2 / Indirect ("Table") Mode

Current spec's `scxstp` §2.2–2.2.3 (pp. 6-8) vs. basis spec's `mcx_selector`/`cx_status`/`scx_table`/`cx_index` §2.2 (pp. 12-17).

| Concept | Basis spec | Current TG spec | Note |
|---|---|---|---|
| Enable/mode | `mcx_selector.version` (0=off, 1=on), bundled into the selector register | `scxstp.mode` (0/1/2), a dedicated register | Mode control split out from the selector |
| Current selection | `mcx_selector` — M-mode only; carries version + `cxe` + `cxu_id` + `state_id` | `cxsel` — URO, unprivileged (via `cxsetsel` only) | No more version or trap-enable field in the register itself |
| CX ID / context index | `cxu_id`[7:0], `state_id`[23:16] — non-adjacent | `CXID`[7:0], `IDX`[15:8] — adjacent | Same 8+8 bit widths, repacked |
| Error reporting | `cx_status` (0x801) — 7 accrued error flags, `fcsr`-like, all privileges | absent — plain illegal-instruction trap per bad access | Accumulator model dropped |
| Table base pointer | `scx_table` (0xBC1), full address¹ | `scxstp` Indirect mode — S-mode, PPN, shares the mode register | ¹M-mode (although name suggests S) |
| Table index register | `cx_index` (0x800), all privileges; write **eagerly loads & caches** the entry into `mcx_selector` | `cxsel` used directly as index; no separate register | Basis spec caches on select; current spec (tentatively) re-resolves per dispatch |
| Table entry format | 32-bit, full selector layout incl. per-entry `cxe` trap bit | 32-bit: `V`[31], `IDX`[15:8], `CXID`[7:0] — single valid bit | `V` plausibly serves the same role as `cxe` (see `V`-bit note below) |
| Table size | 4 KiB, 1024 × 32-bit | 4 KiB, 1024 × 32-bit | Identical, carried forward |

¹ The basis spec contradicts itself on `scx_table`'s privilege: §1.4.2 intro calls it "the supervisor mode `scx_table` CSR" (p. 6, matching the `s`-prefix), but §2.2.3's formal definition says "read and written in machine level" (p. 16). The address itself indicates M-mode — per standard RISC-V CSR-address encoding, bits `[9:8]` of `0xBC1` (`1011 1100 0001`) decode to `11` = Machine, as does `mcx_selector`.

### Historical guidance for open Discussion items (labeled, non-normative)

Basis-spec observations that may inform, not resolve, existing **Open** items — TG confirmation still needed either way.

- **Re: "Indirect mode `cxsel` bounds."** Basis spec's `cx_index` write is *eager* (resolves and caches immediately, catching a bad index once at selection time). The project's current tentative reading of `cxsel` in Indirect mode is *lazy* (re-resolved per dispatch, nothing cached) — a real architectural difference, offered as lineage context only.
- **Re: error-signaling model.** No open question here — current spec's uniform illegal-instruction trap is a deliberate, documented simplification of the basis spec's `cx_status` accumulator. Noted for completeness.
- **Re: "Exclusive vs. shared CX instances on an OS supporting only one model" / `V`-bit intended use.** Basis spec describes using the per-entry `cxe` bit to virtualize logical CX contexts atop fewer physical ones (§2.2.1 discussion, p. 14): clear the trap bit while leaving `cxu_id`/`state_id` intact, trap on next use, restore, re-arm, retry. The current spec's single `V` bit plausibly plays the same dual role — nothing in the spec requires `CXID`/`IDX` to be cleared alongside `V` — but the current spec's own text for `V` (§2.2.3) is purely mechanical ("clear ⇒ illegal instruction") with no stated eviction/lazy-restore rationale. Treat the eviction-marker usage as an unconfirmed project assumption inspired by the basis spec's `cxe` pattern, not as current-spec-stated behavior.

---

*Additional sections may be appended here as future TG meetings cover other chapters against the basis spec.*
