# Research notes

Public examples mention `data-is-muted` and accessible action labels, but they
are not a Google-supported API. Observed examples are not proof that the same
markup is used today, in Polish, or in all meeting lifecycle phases. In
particular the pre-join screen can include microphone controls. The interpreter
here is a testable hypothesis, not a selector to ship.

Before integration: capture consented, sanitized examples from real joined,
pre-join and ended sessions in English and Polish; check mutation behavior for
clicks, keyboard shortcuts, other extensions and UI redraws; test ambiguity,
permissions errors and disconnections. Never save meeting identifiers or content.
