Greetings, RISC-V custom extension reuse champions. To help everyone come up to speed, this message summarizes the purpose, history, and status of this composable custom extensions (CX) task group.

First, essential reading: everyone should get familiar with the TG's draft charter [1]. Please take a moment to review it once again. It is succinct: this TG aims to specify: priv and unpriv ISA specs, and non-ISA specs, for "a plug-and-play ecosystem" of composable custom extensions' hardware and software IP.

   [1] Composable Custom Extensions Task Group Charter, draft, 2024, https://riscv.atlassian.net/wiki/spaces/CCEX/pages/45679102

# Timeline
* 2018: On isa-dev, Guy Lemieux ponders multiplexing RISC-V custom extensions via a hart's-current-extension CSR.
* 2019: SoftCPU SIG forms, notes key gap for the RISC-V FPGA ecosystem:  a common logic interface for custom instructions' "custom function units". By 2021, we proposed:
    1) CX multiplexing: for decentralized, unlimited, conflict-free mix-and-match reuse of n>1 custom extensions;
    2) uniform CX ** state management ** in the API, ABI, OS, and logic interface;
    3) uniform hardware CX unit (CXU) composition.
* 2022: SIG's Draft RISC-V Composable Custom Extensions spec [2][3] writes this up. This is now the "basis spec" for this CX TG -- "one way to do it".
* 2023-2024: RVI CX Task Group proposal [4], CX TG formation, mailing list [5], draft charter [1], requirements. Led by Darius Rad and myself, now led by Guy.
* 2024-2026: Draft spec work [6][7].

    [2] Draft Proposed RISC-V Composable Custom Extensions Specification, 2022, https://raw.githubusercontent.com/grayresearch/CX/28-cx-state-context-csrs/spec/spec.pdf.
    [3] Design and Rationale of the Draft Proposed RISC-V Composable Custom Extensions Specification, 2023, www.youtube.com/watch?v=7daY_E2itpo (1 hr).
    [4] RISC-V Composable Extensions Task Group Proposal, 2024, https://www.youtube.com/watch?v=OgdtLnT-64g (14 mins).
    [5] CX TG mailing list: https://lists.riscv.org/g/tech-composable-custom-extensions 
    [6] CX TG new draft spec w-i-p, repo, 2024-2026, https://github.com/riscv/composable-custom-extensions.
    [7] CX TG new draft spec w-i-p, PDF (attached).

# CX TG Status -- my opinion
* Group shortcomings: needs new strategic members; charter still draft; ratification plan submitted but not reviewed, nor approved.
* Draft TG spec incomplete: some sections OK, some preliminary, some absent; some consensus, some disagreement.

# Status of draft CX TG spec work-in-progress

The CX TG is not bound by the SIG's 2022 draft spec, and we intentionally broadened the TG and esp. the CX criteria to encompass more diverse custom extensions, more kinds custom instructions and extension state.

The spec is an unfinished work in progress. Some ideas are mature, some are strawman, some are absent. Some spec-writing is mature, some is half-done, some is not started. It's uneven and merits a full rewrite pass. In general, Darius favored a brief, concise, precise spec, with any rationale / explanation / exploration elsewhere, perhaps in a rationale chapter at the end. The current spec does not reflect that structure. Each section has a mix of normative and non-normative material.

Here I'll comment on major sections of the spec, in the order of charter deliverables, and give each an arbitrary Jan's-opinion-done-ness letter grade A/B/C/D/F.

## Intro -- placeholder -- F

## Composability criteria -- https://github.com/riscv/composable-custom-extensions/blob/main/src/criteria.adoc -- B

This section specifies the set of rules that determine which RISC-V custom extensions are deemed "composable", forming a subcategory of custom extensions known as composable custom extensions.

I think this section's ideas are well along, and incorporate important new kinds of CX custom instructions beyond that in the SIG's spec, such as access to vector state, custom load/store instructions, etc.. I want to revise it to integrate notions of atomic and idempotent custom instructions.

## Unpriv ISA -- https://github.com/riscv/composable-custom-extensions/blob/main/src/isa-unpriv.adoc -- A

This section specifies unpriv CX multiplexing -- new cxsel CSR, cxsetsel instruction, selector values, valid/invalid, effect on custom instructions and custom CSR accesses. In good shape.

## Unpriv ISA -- state management -- https://github.com/riscv/composable-custom-extensions/blob/main/src/isa-state.adoc -- B

This section specifies unpriv CX state management -- providing an extension-agnostic, uniform way to initialize, manage, save, and restore the state context of any instance of a composable custom extension. In good shape but some open issues / disagreements on means to zero / discard / and/or to reinitialize a state context -- see also scxdiscard.

## Priv ISA -- access control, state context management -- https://github.com/riscv/composable-custom-extensions/blob/main/src/isa-priv.adoc -- B/D

This section specifies priv means to grant/deny access to CX instances to harts, means to track off/init/clean/dirty status of CX instances, means to trap&emulate absent CX instances, means for priv software to do resource management of all the CX instances the hardware is provisioned with, lazy save/restore of CX state contexts, and other good things, across a topology of clusters of processors in a system, etc. The spec incorporates one good proposal for how to do much of this, but we do not yet have consensus on this. (Thus "B/D".) I favor an alternative, single, privileged selection table based approach, reflecting the scx_table/cx_index/mcx_selector design in the SIG's basis spec. I think it is sufficient, simpler (at addressing all the requirements), superior in generality, and less expensive in gates/LUTs. I also favor a different approach for clean/dirty status tracking across all the hart's CX instances, which reflects my bias on where this state should live, CPUs vs CXUs. (I do respect the present spec text -- clean, complete, and self-consistent -- I just prefer a different approach.)

I think this is the most challenging part of the ISA spec, with the biggest impact on the ability to implement and upstream CX support into Linux.

## CX API, ABI, programming model, versioning story -- https://github.com/riscv/composable-custom-extensions/blob/main/src/api.adoc -- B -- except ABI and versioning story -- C

This section specifies a recommended CX API (application programming interface) that CX libraries use to discover, select, use, share, and manage composable custom extensions. The API provides common, uniform CX programming services, including CX naming, discovery, versioning, use, state models, error handling, and resource management. The CX API promotes a CX programming model providing dependable composition of CX libraries, decentralized development, and stable CX library binaries, for modern software ecosystems and applications.

I think this is in fairly good shape. Last year we came to a compromise on whether a hart (i.e., the various CX libraries composed on a thread on that hart) -- can take advantage of more than one instance of a given CX, proposing shared vs. exclusive CX instances, supported in the API, and its impact on the CX ABI and calling convention.

The spec presently contains two overlapping calling convention specs, here and in the External Specs section -- must resolve.

We have yet to review the CX versioning section.

## Logic interface -- https://github.com/riscv/composable-custom-extensions/blob/main/src/li.adoc -- placeholder -- F

This section will specify at least one logic interface by which extensible processors can dependably, automatically integrate one or more composable custom extension units. Leading candidates include the simple CXU-LI from the basis spec, and the powerful but more complex CV-X-IF.

FYI we compared and contrasted RoCC, CV-X-IF, CXU-LI, and SCAIE-V in the 2025-01-31 Extension Logic Interface Workshop [8][9].

    [8] RISC-V CX TG: Extension Logic Interface Workshop, 2025, https://www.youtube.com/watch?v=YtdVpkCIXtE.
    [9] Workshop notes and presentations, 2025, https://riscv.atlassian.net/wiki/spaces/CCEX/pages/202178570/2025-01-31+Extension+Logic+Interface+Workshop+ordinary+meeting+Notes.

## Others -- including https://github.com/riscv/composable-custom-extensions/blob/main/src/external.adoc and https://github.com/riscv/composable-custom-extensions/blob/main/src/cx.rst.

Other spec sections, such as External and propose, e.g. devicetree, ACPI tables, discovery, SBI, Linux userspace API, hwprobe, prctl. The latter are presumably what the CX API runtime will be built atop. Not yet reviewed by the TG.

## Prototyping?

Brandon Freiburger, UBC, built a QEMU + simplified Linux prototype of the SIG's basis spec design. There are several soft and hard FPGA processors that implement subsets of the SIG's basis spec. But to my knowledge the present CX TG spec has not been prototyped in emulation or hardware yet. 

# Wrapping up

CX ISA and interop standards will bring routine, dependable and decentralized development and reuse of custom extensions, their software libraries, and their hardware implementations, for a mix-and-match accelerated solutions ecosystem. But there is still a ton of design, spec, software, prototyping, and other work ahead, to refine and prove this design and get it right for the decades of interop to come.

Well, that's my take. What do you think? Questions, comments, concerns, disagreements?

Thank you.
Jan Gray | individual
