# MeetOverlay — research in progress

This repository currently contains only an experimental, fail-closed interpreter
for possible Google Meet microphone controls. It is **not an application** and
must not be relied upon to determine whether a microphone is live. No extension,
Native Messaging host, overlay, settings panel, or package is available yet.

The input examples are synthetic and have not been validated in a live meeting.
Google Meet has no documented stable DOM contract for these controls; actual
markup, translations, call lifecycle, and dynamic mutations must be verified
before selecting an implementation. The safe result for ambiguity is unknown.

Run the preliminary tests with `node --test tests/*.test.mjs`.

An experimental Qt Core Native Messaging frame decoder is also included. It is
not a native host and does not communicate with a browser. Build and run its
unit tests with `cmake -S . -B build && cmake --build build && ctest --test-dir build`.
