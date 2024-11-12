# Sorcer-ESP
Arduino sketch targeting a XIAO ESP32S3 dev board for controlling an animatronic head. magically 🧙‍♂️

| Command              | Syntax                | Args                                              |
|----------------------|-----------------------|---------------------------------------------------|
| Actuator reset       | A/RST                 |                                                   |
| Actuator speed       | A/SPD>[num]           | [num (0-100)]                                     |
| Actuator up          | A/UPP[>B]             | None (def non-block) or [B] (block)               |
| Actuator down        | A/DWN[>B]             | None (def non-block) or [B] (block)               |
| Actuator middle      | A/MID[>B]             | None (def non-block) or [B] (block)               |
| Actuator unload      | A/UNL                 |                                                   |
| Actuator tilt left   | A/TLL[>[num][,B]]     | None (def 1000, non-block) or [num (0-1000)], [B] |
| Actuator tilt right  | A/TLR[>[num][,B]]     | None (def 1000, non-block) or [num (0-1000)], [B] |
| Actuator bounce      | A/BNC                 |                                                   |
| Actuator shake       | A/SHK[>[num]]         | None (def 2) or [num (1-20)]                      |
| Jaw speed            | J/SPD>[num]           | [num (0-100)]                                     |
| Jaw open             | J/OPN[>B]             | None (def non-block) or [B] (block)               |
| Jaw close            | J/CLS[>B]             | None (def non-block) or [B] (block)               |
| Jaw color custom hex | J/C>#[hex]            | [hex] (def both), opt [L or R]                    |
| Eye color green      | E/C/GRN[>[L or R]]    | None (def both) or [L or R]                       |
| Eye color red        | E/C/RED[>[L or R]]    | None (def both) or [L or R]                       |
| Eye color blue       | E/C/BLU[>[L or R]]    | None (def both) or [L or R]                       |
| Eye color yellow     | E/C/YLW[>[L or R]]    | None (def both) or [L or R]                       |
| Eye color purple     | E/C/PRP[>[L or R]]    | None (def both) or [L or R]                       |
| Eye color orange     | E/C/ORG[>[L or R]]    | None (def both) or [L or R]                       |
| Eye color custom hex | E/C>#[hex][,[L or R]] | [hex] (def both), opt [L or R]                    |
| Eye brightness       | E/B>[num]             | [num (0-255)]                                     |
| Eye reset            | E/R                   |                                                   |
| Eye open             | E/D/OPN[>[L or R]]    | None (def both) or [L or R]                       |
| Eye close            | E/D/CLS[>[L or R]]    | None (def both) or [L or R]                       |
| Eye dilate           | E/D/DIL[>[L or R]]    | None (def both) or [L or R]                       |
| Eye contract         | E/D/CTR[>[L or R]]    | None (def both) or [L or R]                       |
| Eye squint           | E/D/SQT[>[L or R]]    | None (def both) or [L or R]                       |
| Eye set infill       | E/D/INF>[Y or N]      | [Y or N]                                          |
| Eye dead             | E/D/DIE               |                                                   |
| Eye rainbow          | E/D/RNB               |                                                   |
| Eye confused         | E/D/CNF               |                                                   |
| Eye blink            | E/A/BLK               |                                                   |
| Eye wink             | E/A/WNK>[L or R]      | [L or R]                                          |
| Eye spiral dot       | E/A/SPD               |                                                   |
| Eye spiral line      | E/A/SPL               |                                                   |
| Button enable        | B/ENA                 |                                                   |
| Button disable       | B/DIS                 |                                                   |
| Reset                | R                     |                                                   |
