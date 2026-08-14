# Wine and Proton compatibility

Phase 2 uses clean-room, open-source compatibility projects only:

- Wine for PE loading and Win32 APIs.
- Proton for gaming-oriented integration.
- DXVK and VKD3D-Proton for Direct3D translation.
- FAudio for XAudio-compatible game audio.

No Windows source, headers, binaries, or ReactOS source may be introduced.
Runtime execution remains `not-run` until Wine/Proton is packaged for NovaOS.