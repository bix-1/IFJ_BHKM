&nbsp;
<center> <h1> Projektová dokumentácia k implementácii prekladača imperatívneho jazyka IFJ20</h1> </center>
&nbsp;
<center> <h1>Tím 014, varianta II </h1> </center>
&nbsp;
<center> <h4> 9. december 2020</h4> </center>
&nbsp;

## Autori:
- **Hladký Tomáš** xhladk15@stud.fit.vutbr.cz		%body

- Kostolányi Adam xkosto04@stud.fit.vutbr.cz			%body		

- Makiš Jozef xmakis00@stud.fit.vutbr.cz			%body

- Bartko Jakub xbartk07@stud.fit.vutbr.cz			%body
&nbsp;

## Úvod 

Cieľom našej dokumentácie je popísať postup riešenia projektu z predmetov IFJ a IAL. Projekt implementácia prekladača imperatívneho jazyka IFJ20, prekladá jazyk IFJ20, ktorý je podmnožinou jazyka GO, ktorý prekladá zo zdrojového súboru do cieľového jazyka IFJcode20.  Postupne v tejto dokumentácii bude popis vývoja jednotlivých časti nášho prekladača, ktorý generuje výsledný medzikód na štandartný výstup. Takisto bude popísaná práca v tíme medzi jednotlivými členmi týmu.

## Návrh a implementácia
### Lexikálna analýza
Po oboznámení sa s deterministickým konečným automatom na prednáške sme začali s návrhom DKA pre našu lexikálnu analýzu. Začali sme tým že sme si určili jednotlivé lexémy, ktoré budeme rozpoznávať pomocou DKA. Každý konečný stav nášho DKA predstavuje jeden typ lexému. Z lexikálnych jednotiek sme si vytvorili tokeny, ktoré okrem typu lexému obsahujú aj jeho atribút v prípade identifikátoru. Hlavnú časť našej lexikálnej analýzy obsahuje funkcia `get_next_token(tToken *token)`. Funkcia prijíma vo svojom parametri štruktúru tokena, ktorá obsahuje jeho typ a prípadne jeho atribút. Hlavnú časť funkcie obsahuje switch, ktorý je vo vnútri nekonečného cyklu. Tento nekonečný cyklus beží do vtedy pokiaľ nenačíta token, znak konca súboru alebo lexikálnu chybu. Switch sa riadi jednotlivými stavmi, ktoré môžu nastav počas behu programu. Jednotlivé stavy switchu predstavujú stavy v našom DKA. Lexikálna analýza využíva pre ukladanie atribútov dynamické pole , kde sa ukladajú jednotlivé znaky načítané zo štandardného vstupu. Po načítaní je výraz prekopírovaný do atribútu tokena. Všetky definované tokeny a stavy môžme nájsť v súbore `scanner.h`. Implementáciu môžme nájsť v súbore `scanner.c`. Jednotlivé tokeny ďalej spracúva parser.

### Syntaktická analýza

### Sémantická analýza

### Generátor výsledného medzikódu v jazyku IFJcode20

## Algoritmy a dátové štruktúry
### Dynamický reťazec
Pre načítavanie vopred neznámeho reťazca sme nemohli používať statické pole. Z tohoto dôvodu sme sa rozhodli používať dynamické pole, ktoré nám umožňuje načítavať pole o neznámej dĺžke. Dynamický reťazec prestavuje štruktúra reťazca v súbore `str.h`. Táto štruktúra obsahuje ukazovateľ na pole znakov `char *str`. Dĺžku reťazca `int str_lenght`, ktorá nám pomáha pri alokácii pamäte. Veľkosť momentálnej alokovanej pamäte je v premennej `int str_alloc_size` Ďalej dynamický reťazec obsahuje funkcie ako `str_init(string *s)`, ktorá slúži pri vytváraní nového reťazca. `str_free(string *s)` sa používa pri uvoľňovaní alokovanej pamäte dynamického reťazca. Funkciu `str_clear(string *s)` používame pri čistení reťazca, `str_add_char(string *s, char c)` slúži k pridaniu načítaného znaku zo štandardného vstupu do nášho reťazca. Funckia`str_copy(string *s1, string *s2)` kopíruje už načítaný reťazec 
z dynamického pola do atribútu tokena. `str_cmp_cons(string *s, char *s_const)` porovnáva už načítaný reťazec s konštantou, ktorá môže byť napríklad kľúčové slovo, ktoré nechceme aby sa objavilo medzi identifikátormi. Tú to knižnicu sme si prevzali z zjednodušeného interpretu, ktorý sa nachádzal medzi študijnými materiálmi.

### Tabuľka s rozptýlenými položkami 

### Escape formát
Escape formát modul slúži na konvertovanie načítaného reťazca scannerom na formát, ktorý je vhodný na výpis pre generátor výsledného medzikódu. Ide hlavne o znaky ASCII s kódom 000-032, 035 a 092. Tento modul obsahuje dve funkcie a to `merge_str(char *str, size_t start_i, int move)`a `escape_reformat(char *string)`. Merge_str spája koniec reťazca 
s prekonvertovanou escape sekvenciou. Funkcia escape_reformat prijíma už načítaný reťazec, ktorý mu predáva scanner. Reťazec je následne prekonvertovaný do tvaru _\𝑥𝑦z_, kde _xyz_ je dekadické číslo konkrétneho znaku.

### Zásobník pre syntaktickú analýzu
Zásobník pre syntaktickú analýzu slúži na ukladanie a prácu s elementami. Obsahuje dve štruktúry `stackElem` a `tokenStack`. Štruktúra `stackElem` obsahuje token a ukazovateľ na nasledujúci token. Štruktúra`tokenStack`obsahuje element ktorý sa nachádza na vrchole zásobníka a následujúci element. Funkcia `stack_init(tokenStack *stack)`slúži na inicializáciu zásobníka. Ďalej zásobník obsahuje rôzne operácie ako je `stack_push(tokenStack *stack, tToken tokPush)`, ktorá vloží token do zásobníka.  Keď chceme token dostať zo zásobníka použijeme`stack_pop(tokenStack *stack)`. Následne môžme zásobník vyčistiť pomocou funkcie `stack_empty(tokenStack *stack)`, alebo uvoľniť z pamäte vďaka funkcii `stack_free(tokenStack *stack)`.

### List funkcii

### Zásobník pre generátor výsledného medzikódu.
Zásobník pre generátor výsledného medzikódu sa používa na ukladanie unikátnych hodnôt pre skoky v podmienených výrazoch a cykloch. Obsahuje dve štruktúry `jmp_label_stack_t`a `jmp_label_stack_top_t`. Štruktúra `jmp_label_stack_t`obsahuje ukazovatele na predchádzajúce a následujúce návestie. Štruktúra `jmp_label_stack_top_t`obsahuje ukazovateľ na vrchol zásobníka s návestiami. Zásobník obsahuje inicializačnú funkciu `jmp_label_stack_init`, ktorá inicializuje zásobníky pre podmienene výrazy a cykly. Obsahuje funkciu `jmp_label_stack_push(jmp_label_stack_top_t *top, int value)`, ktorá vkladá návestie na zásobník aj s jeho unikátnou hodnotou vygenerovanou v generátore. Ďalej používame funkciu`jmp_label_stack_pop(jmp_label_stack_t *stack, jmp_label_stack_top_t *top)`, ktorá vysunie návestie zo zásobníka pokiaľ nie sme na jeho dne a vráti hodnotu návestia. Funkcia `jmp_label_stack_top(jmp_label_stack_top_t *top)` vracia hodnotu vrcholu zásobníka. Na následné vyčistenie zásobníka používame funkciu 
`jmp_label_stack_clear(jmp_label_stack_t *stack, jmp_label_stack_top_t *top)`. Na uvoľnenie konkrétneho zásobníka poslúži funkcia 
`jmp_label_stack_free(jmp_label_stack_t *stack, jmp_label_stack_top_t *top)`. A ak chceme uvoľniť kompletne všetky zásobníky použijeme  `jmp_label_stack_free_all()` .
## Práca v týme 
## Záver
## Prílohy
## Literatúra
Prezentácie z predmetov IFJ a IAL.

<!--stackedit_data:
eyJoaXN0b3J5IjpbLTEzNjUzMjU1NywtMTA3NTAxMDMzMiwxNT
M5NjMzMjI4LC0zOTYwNzI2NjIsLTg1MjEyNTIyNSwtNjY3NTg0
OTI1LC0yMTEwMTU0MzYzLDYwMDc1MjU1LC00Njc4NTM2NDIsLT
EwMjk2NTI3NzEsNzgwMDMwMzUxLDExNjA2MjgzNzYsLTY2Njg1
MDkxNywxNDkwNDY0NzkzLDEwMTczOTQxMTMsMjAyODI3NDc2Mi
wxNjI3Mjg3MzAxLDUxNjUxNjUxNiwtNDEwMTcwNzAzLDIwMDc0
ODAxNDFdfQ==
-->