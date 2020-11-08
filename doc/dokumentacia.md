# Projektová dokumentácia k implementácii prekladača imperatívneho jazyka IFJ20
## Tím 014, varianta II
## Autori:
- **Hladký Tomáš** xhladk15@stud.fit.vutbr.cz		%body

- Kostolányi Adam xkosto04@stud.fit.vutbr.cz			%body		

- Makiš Jozef xmakis00@stud.fit.vutbr.cz			%body

- Bartko Jakub xbartk07@stud.fit.vutbr.cz			%body

## Úvod 

Cieľom našej dokumentácie je popísať postup riešenia projektu z predmetov IFJ a IAL. Projekt implementácia prekladača imperatívneho jazyka IFJ20, prekladá jazyk IFJ20, ktorý je podmnožinou jazyka GO zo zdrojového súboru do cieľového jazyka IFJcode20.  Postupne v tejto dokumentácii bude popis vývoja jednotlivých časti nášho prekladača, ktorý generuje výsledny medzikód na štandartný výstup. Takisto bude popísaná práca v tíme medzi jednotlivými členmi týmu.
## Návrh a implementácia
### Lexikálna analýza
Po oboznámení sa s deterministickym konečným automatom na prednáške sme začali s návrhom DKA pre našu lexikálnu analýzu. Začali sme tým že sme si určili jednotlivé lexémy, ktoré budeme rozpoznávať pomocou DKA. Každý konečný stav nášho DKA predstavuje jeden typ lexému. Z lexikálnych jednotiek sme si vytvorili korešpodujúce tokeny, ktoré okrem typu lexému obsahujú aj jeho atribút v prípade identifikátoru. Hlavnú časť našej lexikálnej analýzi obsahuje funkcia _get_next_token(tToken *token)_. Funkcia prijíma vo svojom parametri štruktúru tokena, ktorá obsahuje jeho typ a prípadne jeho atribút. Hlavnú časť funkcie obsahuje switch, ktorý je vo vnútri nekonečného cyklu. Tento nekonečný cyklus beží do vtedy pokiaľ nenačíta token, znak konca súboru alebo lexikálnu chybu. Switch sa riadi jednotlivými stavmi, ktoré môžu nastav počas behu programu. Jednotlivé stavy switchu predstavujú stavy v našom DKA. Lexikalna analýza využíva pre ukladanie atribútov dynamické pole charov, kde sa ukladajú jednotlivé znaky načítané zo štandartneho vstupu. Tú to knižnicu sme si prevzali z zjednodušeného interpretu, ktorý sa nachádzal medzi študijnými materialmi. Po načítaní je výraz prekopírovaný do atribútu tokena. Všetky definované tokeny a stavy môžme nájsť v súbore *scanner.h*. Implementáciu môžme nájsť v súbore *scanner.c*. Jednotlivé tokeny ďalej spracúva parser.

## Algoritmy a dátové štruktúry
### Dynamický reťazec
Pre načítavanie vopred neznámeho reťazca sme nemohli používať statické pole. Z tohoto dôvodu sme sa rozhodli používať dynamické pole, ktoré nám umožňuje načítavať pole o neznámej dĺžke. Dynamický reťazec prestavuje štruktúra string v súbore *str.h*. Táto štruktúra obsahuje ukazovateľ na pole znakov _char *str_. Dĺžku reťazca _int str_lenght_, ktorá nám pomáha pri alokácii pamäte. Veľkosť momentálnej alokovanej pamäte je v premennej _int str_alloc_size_ Ďalej dynamický reťazec obsahuje funkcie ako _str_init(string *s)_, ktorá slúži pri vytváraní nového reťazca. _str_free(string *s)_ sa používa pri uvolňování alokovanej pamäte dynamického reťazca. Funkciu _str_clear(string *s)_ používame pri čistení reťazca, _str_add_char(string *s, char c)_ slúži k pridaniu načítaného znaku zo štandardného vstupu do nášho reťazca. _str_copy(string *s1, string *s2)_ kopíruje už načítaný reťazec z dynamického pola do atribútu tokena. _str_cmp_cons(string *s, char *s_const)_ porovnáva už načítaný reťazec s konštantou, ktorá môže byť napríklad kľúčové slovo, ktoré nechceme aby sa objavilo medzi identifikátormi. 
## Práca v týme 
## Záver
## Prílohy
## Literatúra
Prezentáciue z predmetov IFJ a IAL.
Manualové stránky.
<!--stackedit_data:
eyJoaXN0b3J5IjpbMTg3Nzk5NDQ2NSwxOTY1OTQ5MTE4LC01NT
Y1NTcxNzgsLTM5NTg1MjQzMSwxMDUwMTIxODQsMTU2MTc0OTA2
NywyMTM1MzI3MTA4LC0xMDkxNDI0ODI0LC02NTA0MzcxMjZdfQ
==
-->