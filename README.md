# Sorcer-ESP
Arduino sketch targeting a XIAO ESP32S3 dev board for controlling an animatronic head. magically 🧙‍♂️

## Serial Command Reference
| Command              | Syntax                | Args                           |
|----------------------|-----------------------|--------------------------------|
| Actuator up          | A/UP                  |                                |
| Actuator down        | A/DWN                 |                                |
| Actuator middle      | A/MID                 |                                |
| Actuator unload      | A/UNL                 |                                |
| Actuator tilt left   | A/TLL                 |                                |
| Actuator tilt right  | A/TLR                 |                                |
| Actuator bounce      | A/BNC                 |                                |
| Actuator shake       | A/SHK[>[num]]         | None (def 2) or [num]          |
| Jaw open             | J/OPN                 |                                |
| Jaw close            | J/CLS                 |                                |
| Eye color green      | E/C/GRN[>[L or R]]    | None (def both) or [L or R]    |
| Eye color red        | E/C/RED[>[L or R]]    | None (def both) or [L or R]    |
| Eye color blue       | E/C/BLU[>[L or R]]    | None (def both) or [L or R]    |
| Eye color yellow     | E/C/YLW[>[L or R]]    | None (def both) or [L or R]    |
| Eye color purple     | E/C/PRP[>[L or R]]    | None (def both) or [L or R]    |
| Eye color orange     | E/C/ORG[>[L or R]]    | None (def both) or [L or R]    |
| Eye color custom hex | E/C>#[hex][,[L or R]] | [hex] (def both), opt [L or R] |
| Eye brightness       | E/B>[num]             | [num]                          |
| Eye reset            | E/R                   |                                |
| Eye open             | E/D/OPN[>[L or R]]    | None (def both) or [L or R]    |
| Eye close            | E/D/CLS[>[L or R]]    | None (def both) or [L or R]    |
| Eye dilate           | E/D/DIL[>[L or R]]    | None (def both) or [L or R]    |
| Eye contract         | E/D/CTR[>[L or R]]    | None (def both) or [L or R]    |
| Eye squint           | E/D/SQT[>[L or R]]    | None (def both) or [L or R]    |
| Eye set infill       | E/D/INF>[Y or N]      | [Y or N]                       |
| Eye dead             | E/D/DIE               |                                |
| Eye rainbow          | E/D/RNB               |                                |
| Eye confused         | E/D/CNF               |                                |
| Eye blink            | E/A/BLK               |                                |
| Eye wink             | E/A/WNK>[L or R]      | [L or R]                       |
| Eye spiral dot       | E/A/SPD               |                                |
| Eye spiral line      | E/A/SPL               |                                |
| Reset                | R                     |                                |