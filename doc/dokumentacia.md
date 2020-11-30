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
Po oboznámení sa s deterministickým konečným automatom na prednáške sme začali s návrhom DKA pre našu lexikálnu analýzu. Začali sme tým že sme si určili jednotlivé lexémy, ktoré budeme rozpoznávať pomocou DKA. Každý konečný stav nášho DKA predstavuje jeden typ lexému. Z lexikálnych jednotiek sme si vytvorili tokeny, ktoré okrem typu lexému obsahujú aj jeho atribút v prípade identifikátoru. Hlavnú časť našej lexikálnej analýzy obsahuje funkcia `get_next_token`. Funkcia prijíma vo svojom parametri štruktúru tokena, ktorá obsahuje jeho typ a prípadne jeho atribút. Hlavnú časť funkcie obsahuje switch, ktorý je vo vnútri nekonečného cyklu. Tento nekonečný cyklus beží do vtedy pokiaľ nenačíta token, znak konca súboru alebo lexikálnu chybu. Switch sa riadi jednotlivými stavmi, ktoré môžu nastav počas behu programu. Jednotlivé stavy switchu predstavujú stavy v našom DKA. Lexikálna analýza využíva pre ukladanie atribútov dynamické pole , kde sa ukladajú jednotlivé znaky načítané zo štandardného vstupu. Po načítaní je výraz prekopírovaný do atribútu tokena. Všetky definované tokeny a stavy môžme nájsť v súbore `scanner.h`. Implementáciu môžme nájsť v súbore `scanner.c`. Jednotlivé tokeny ďalej spracúva parser.

### Syntaktická analýza

### Sémantická analýza

### Generátor výsledného medzikódu v jazyku IFJcode20

## Algoritmy a dátové štruktúry

### Dynamický reťazec
Pre načítavanie vopred neznámeho reťazca sme nemohli používať statické pole. Z tohoto dôvodu sme sa rozhodli používať dynamické pole, ktoré nám umožňuje načítavať pole o neznámej dĺžke. Dynamický reťazec prestavuje štruktúra reťazca v súbore `str.h`. Táto štruktúra obsahuje ukazovateľ na pole znakov `char *str`. Dĺžku reťazca `int str_lenght`, ktorá nám pomáha pri alokácii pamäte. Veľkosť momentálnej alokovanej pamäte je v premennej `int str_alloc_size` Ďalej dynamický reťazec obsahuje funkcie ako `str_init`, ktorá slúži pri vytváraní nového reťazca. `str_free` sa používa pri uvoľňovaní alokovanej pamäte dynamického reťazca. Funkciu `str_clear` používame pri čistení reťazca, `str_add_char` slúži k pridaniu načítaného znaku zo štandardného vstupu do nášho reťazca. Funkcia`str_copy` kopíruje už načítaný reťazec z dynamického pola do atribútu tokena. `str_cmp_cons` porovnáva už načítaný reťazec s konštantou, ktorá môže byť napríklad kľúčové slovo, ktoré nechceme aby sa objavilo medzi identifikátormi. Tú to knižnicu sme si prevzali z zjednodušeného interpretu, ktorý sa nachádzal medzi študijnými materiálmi.

### Tabuľka s rozptýlenými položkami 
Pre implementáciu tabuľky symbolov sme si vybrali variantu druhú, ktorá sa realizuje pomocou tabuľky s rozptýlenými položkami. Vybrali sme si ju preto, lebo sme s ňou už boli oboznámený z voliteľného predmetu IJC. Vďaka tomu sme mohli použiť časť, ktorá bola už implementovaná v projekte z tohoto predmetu pre našu tabuľku symbolov. Tabuľka obsahuje rôzne typy štruktúr, ktoré nám pomáhajú uchovávať premenné, funkcie a ich rôzne dátové typy, ktoré sa môžu v programe vyskytnúť. Taktiež obsahuje štruktúry, vďaka ktorým môžme tabuľku prehľadávať, prípadne zistiť v ktorej tabuľke sa nachádzame. Inicializáciu tabuľky tabuľky prevádza funkcia `symtable_init`. Veľkosť tabuľky zisťujeme pomocou funkcie `symtable_size`. Základom je hashovacia funkcia `symtable_hash_fun`. Ďalej tabuľka obsahuje vyhľadávaciu funkciu `symtable_find`. V prípade vkladania prvkov používame funkciu`symtable_insert`na vkladanie prvkov do tabuľky. Pre prípad že chceme zmazať nejaký špecificky prvok z tabuľky využívame funkciu `symtable_erase`. Tabuľka obsahuje rôzne iteratory ako sú `symtable_begin`,  `symtable_iterator_next` a `symtable_end`, vďaka ktorým sa môžme posúvať v tabuľke k jej prvému prvku, nasledujúcemu alebo po poslednom prvku na jej koniec. Pomocou iteratorov môžme z tabuľky čítať, alebo zapisovať vďaka funkciam `symtable_iterator_get_key`, `symtable_iterator_get_value` a `symtable_iterator_set_value`. Pre čistenie tabuľky máme funkciu `symtable_clear` a pre jej uvoľnenie z pamäte `symtable_free`. Tabuľka obsahuje funkcie pre inicializáciu symbolov `elem_init`, `sym_func_init`, `sym_var_item_init`, `sym_var_list_init`,`list_item_init`a ich uvoľnenie `sym_func_free`, `sym_var_item_free`, `sym_var_list_free `,`list_item_free`. Pre nastavovanie typov, dát, konštánt, platnosti, formátov, ďalších a predchádzajúcich prvkov sme implementovali funkcie `sym_var_item_set_type`, `sym_var_item_set_data`, `sym_var_item_set_const`, `sym_var_item_set_global`, `sym_var_item_set_formatted`, `list_item_set_next`, `list_item_set_prev`. Z tabuľky môžme získať kľúč elementu `elem_key`alebo ak potrebujeme veľkosť tabuľky tak používame funkciu `sym_var_list_size`. Pre zisk nasledujúceho prvku a zároveň nastavenie daného prvku na aktívny máme funkciu `sym_var_list_next`. V prípade že používame zásobník chceme predchádzajúci prvok a zároveň ho nastaviť na aktívny čo môžme vďaka funkcii  `sym_var_list_prev `. Funkcia `sym_var_list_get_active` vracia aktívny prvok v tabuľke. Pre zmenu hodnoty, ale nie typu používame funkciu `sym_var_item_change_data`. Zmeniť hodnotu na predvolenú môžme funkciou `sym_var_item_reset` a vyčistenie listu funkciou `sym_var_list_clear`.

### Escape formát
Escape formát modul slúži na konvertovanie načítaného reťazca scannerom na formát, ktorý je vhodný na výpis pre generátor výsledného medzikódu. Ide hlavne o znaky ASCII s kódom 000-032, 035 a 092. Tento modul obsahuje dve funkcie a to `merge_str`a `escape_reformat`. Merge_str spája koniec reťazca s prekonvertovanou escape sekvenciou. Funkcia escape_reformat prijíma už načítaný reťazec, ktorý mu predáva scanner. Reťazec je následne prekonvertovaný do tvaru _\𝑥𝑦z_, kde _xyz_ je dekadické číslo konkrétneho znaku.

### Zásobník pre syntaktickú analýzu
Zásobník pre syntaktickú analýzu slúži na ukladanie a prácu s elementami. Obsahuje dve štruktúry `stackElem` a `tokenStack`. Štruktúra `stackElem` obsahuje token a ukazovateľ na nasledujúci token. Štruktúra`tokenStack`obsahuje element ktorý sa nachádza na vrchole zásobníka a následujúci element. Funkcia `stack_init`slúži na inicializáciu zásobníka. Ďalej zásobník obsahuje rôzne operácie ako je `stack_push`, ktorá vloží token do zásobníka.  Keď chceme token dostať zo zásobníka použijeme`stack_pop`. Následne môžme zásobník vyčistiť pomocou funkcie `stack_empty`, alebo uvoľniť z pamäte vďaka funkcii `stack_free`.

### Zoznam inštrukcii 
Pre prácu s inštrukciami používame dvojsmerne viazaný zoznam. Dvojsmerne viazaný zoznam obsahuje dve štruktúry `instr_t` a `list_t`. Obe štruktúry nám pomáhajú sa orientovať v zozname či už chceme získať ukazovateľ na prví prvok, aktívny, posledný, predchádzajúci, následný, alebo chceme získať jeho hodnotu. Prvú časť zoznamu tvoria operácie zoznamu. Pre inicializáciu zoznamu používame funkciu `list_create`. Následne po inicializácii môžme použiť funkciu `list_add`pre pridanie prvku do zoznamu. Aktívny prvok zistíme vďaka funkcii `list_get_active`, následujúci prvok `list_get_next`. Veľkosť zoznamu overíme pomocou funkcie `list_size`. Čistenie zoznamu zabezpečuje funkcia `list_is_empty`. Ak chceme zoznam zničiť, potom ak ho už nepotrebujeme tak použijeme funkciu `list_destroy`. Druhú časť zoznamu tvoria funkcie, ktoré už priamo pracujú s inštrukciami. Funkcionalita mnohých funkcii je rovnaká ako už hore opísaných. Obsahuje už konkrétnejšie funkcie, ktoré pridávajú elementy do zoznamu `instr_add_elem1`,  `instr_add_elem2`. Získať elemety a ich miesto v zozname môžme vďaka funkciám `instr_get_dest`, `instr_get_elem1` a `instr_get_elem2`. Takisto im môžme nastaviť ich typ cez funkciu `instr_set_type` alebo ho získať`instr_get_type`. 

### Zásobník pre generátor výsledného medzikódu.
Zásobník pre generátor výsledného medzikódu sa používa na ukladanie unikátnych hodnôt pre skoky v podmienených výrazoch a cykloch. Obsahuje dve štruktúry `jmp_label_stack_t` a `jmp_label_stack_top_t`. Štruktúra `jmp_label_stack_t`obsahuje ukazovatele na predchádzajúce a následujúce návestie. Štruktúra `jmp_label_stack_top_t`obsahuje ukazovateľ na vrchol zásobníka s návestiami. Zásobník obsahuje inicializačnú funkciu `jmp_label_stack_init`, ktorá inicializuje zásobníky pre podmienene výrazy a cykly. Obsahuje funkciu `jmp_label_stack_push`, ktorá vkladá návestie na zásobník aj s jeho unikátnou hodnotou vygenerovanou v generátore. Ďalej používame funkciu`jmp_label_stack_pop`, ktorá vysunie návestie zo zásobníka pokiaľ nie sme na jeho dne a vráti hodnotu návestia. Funkcia `jmp_label_stack_top` vracia hodnotu vrcholu zásobníka. Na následné vyčistenie zásobníka používame funkciu `jmp_label_stack_clear`. Na uvoľnenie konkrétneho zásobníka poslúži funkcia `jmp_label_stack_free`. A ak chceme uvoľniť kompletne všetky zásobníky použijeme  `jmp_label_stack_free_all` .

## Tímová spolupráca

### Založenie týmu
Keďže sa už medzi sebou poznáme už od prvého semestru kedy sme boli spolu už v tíme tak založenie tímu nebolo pre nás vôbec zložité. Určitý základ tímovej spolupráce sme nadobudli pri domácich úlohách v prvom semestri z diskrétnej matematiky a lineárnej algebry. Vtedy sme boli nútený sa medzi sebou dohodnúť a komunikovať. No táto spolupráca nebola na úrovní kedy by sme spoločne riešili tímový projekt v ktorom sa programuje. Vedeli sme že nás bude čakať veľký tímový projekt, takže sme sa dohodli že budeme spolupracovať ešte pred začiatkom semestra. 

### Komunikácia
Prvé tímové osobné stretnutie prebehlo v prvom týždni semestra, kedy sme sa bavili o tom čo nás bude čakať. Vtedy sme si ešte nevedeli presne nerozdelili prácu, pretože sme čakali na zadanie projektu.  Bohužiaľ nastala neočakávaná situácia, kedy sa zrušilo prezenčné vyučovanie a boli sme nútený odísť domov. Keďže sme od seba vzdialený stovky kilometrov, museli sme začať komunikovať prostredníctvom internetu. Ako prvé bolo to že sme si založili skupinu na Discorde.  Na prvom stretnutí sme si zvolili svojho lídra tímu. Taktiež sme si rozdelili prácu na jednotlivých komponentov prekladača na základe prvej prednášky, kde sme boli s nimi oboznámený. Zvolili sme si verzovací systém, a založili repozitár. Následné sme si zvolili ďalší termín hovoru, ktorý bol v čase kedy sme mali základné znalosti o našej časti projektu. Komunikácia až do dokončenia projektu prebiehala počas hovorov, prípadne v chate našej skupiny. Prípadne že sa objavili väčšie nejasnosti ohľadom prepojenia jednotlivých komponent tak sa zvolal hovor. Tieto hovory prebiehali už podľa potreby. Taktiež dôležité hovory prebiehali počas nultého, pokusného a finálneho odovzdania, kedy sme všetko kontrolovali či je všetko funkčné a pripravené na odovzdanie.

### Verzovanie programu 
Verzovanie nášho projektu prebiehalo v súkromnom repozitári na GitHube. Projekt sme verzovali podľa predom dohodnutých podmienok aké sú jazyk, formát správ a editor v ktorom budeme spoločne pracovať. Pokiaľ sme objavili nejaké chyby medzi sebou pri kontrole, tak sme ich riešili buď prostredníctvom Discordu alebo pomocou `Issues` na GitHube.

### Rozdelenie
- **Hladký Tomáš** :

- Kostolányi Adam :

- Makiš Jozef :

- Bartko Jakub :
## Záver
Projekt z predmetov IFJ a IAL nás neskutočne obohatil o praktické a teoretické vedomostí 
z oblasti algoritmov a formálnych jazykov a prekladačov. Vyskúšali sme si prácu s dátovými štruktúrami ako sú zásobník, dynamické pole, viazaný zoznam a hashovacia tabuľka. Tieto dátové štruktúry sme využili pri tvorbe prekladača. Zistili sme z čoho sa skladá taký prekladač 
a pochopili princíp činnosti jednotlivých komponent. Mali sme možnosť hlbšie nazrieť do niečoho čo sme využívali doteraz pri programovaní vždy, ale nevedeli ako to presne funguje. Táto skúsenosť nám určite pomôže v budúcnosti pri tvorení, ladení a optimalizácii programov. Taktiež nesmieme zabudnúť na to že sme si vyskúšali tímovú spoluprácu snáď na našom najväčšom projekte. Zistili sme aké je dôležité si veci plánovať a byť zodpovedný pri práci, pretože na našej práci záleží osud celého tímu. Vyskúšali sme si aké je to verzovať a udržovať veľký projekt. Zistili sme aké je dôležité testovať jednotlivé časti projektu, pre správny chod celého programu.

## Prílohy

## Literatúra
Prezentácie z predmetov IFJ a IAL.

<!--stackedit_data:
eyJoaXN0b3J5IjpbOTIxODYzNTExLC01NDYzNTYwNDQsLTEzMz
Q3NjcyMjYsNTUxNDc2ODAwLDEwNTI3NTEzNCwtMTk4MzU2MTQw
OCwtMTI4ODk3ODI1MywyMDAxMTA1ODkyLC0yOTQwMjM3MzUsMT
A0MzY1ODQ5NCwxOTU5MzE4MjMzLC0xODYyMjcwMjY3LC0xNTQw
NTU0OTIwLC05Nzg1MjEyOTcsLTIwNzE0OTA4NTMsMjEyODMyMj
QyNSwxMTAzMzMyNzcyLC0xNTgzNDMyNDU5LC0xMzYwODMzODUx
LDE3MTIxNTMzODddfQ==
-->