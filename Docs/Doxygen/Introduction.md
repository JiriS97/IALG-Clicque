\mainpage Úvod a zadání

## Autoři
| Jméno | Příjmení | Login    |
|-------|----------|----------|
| Jiří  | Šrámek   | xsrame02 |
| Matěj | Kroulík  | xkroul02 |
| Jiří  | Bekr     | xbekrj00 |
| Tomáš | Macko    | xmacko11 |

## Zadání

Klika grafu je podgraf, který je úplným grafem. Kterýkoliv vrchol kliky je tedy spojen hranou se všemi ostatními vrcholy kliky. 
Vytvořte program pro hledání největší kliky v neorientovaném grafu. Pokud existuje více řešení, nalezněte všechna. Výsledky prezentujte vhodným způsobem. Součástí projektu bude načítání grafů ze souboru a vhodné testovací grafy. V dokumentaci uveďte teoretickou složitost úlohy a porovnejte ji s experimentálními výsledky.


### Popis algoritmu
Bron-Kerboschův algoritmus, dále jen B-K algoritmus,
 vyhledává nejvyšší počet klik v neorientovaném grafu.
Algoritmus vymysleli v roce 1973 nizozemští vědci Coenraad Bron a Joep Kebosch, odtud jeho název. Jejich algoritmus byl efektivnější než tehdy používané, díky čemuž nalezl uplatnění hlavně v chemických vědních oborech.
Současně s ním ještě vznikl algoritmus Akkoyunlu, který se sice sémanticky lišil, ale nakonec vedl ke shodnému rekurzivnímu postupu jako B-K algoritmus.
Jev si lze představit jako shluk lidí. B-K algoritmus vyhledá největší skupinu lidí, kteří se navzájem všichni znají, popřípadě kteří mají na sebe navzájem telefonní čísla.
Základní forma B-K algoritmu je rekurzivní funkce se zpětným vyhledáváním, která hledá všechny největší kliky v grafu. Algoritmus má tři parametry – R, P a X. R je výstup algoritmu. P je vstupní graf a X je seznam vyloučených uzlů.
B-K algoritmus je v obecném algoritmovacím jazyce realizován následovně:
\n
\code{.py}
BronKerbosch(R, P, X):
    if P and X are both empty:
        report R as a maximal clique
        return
    for each vertex v in P:
        BronKerbosch(R ⋃ {v}, P ⋂ N(v), X ⋂ N(v))
        P := P \ {v}
       X := X ⋃ {v}
\endcode
Počáteční volání se provádí s prázdným grafem (seznamem uzlů) R a X. Parametr P obsahuje graf, ve kterém se mají hledat kliky. Při každém dalším rekurzivním volání projde algoritmus všechny uzly grafu {v} v P. Pokud je P prázdné a zároveň je prázdné i X, tak nahlásí kliku. V případě že je prázdné pouze P vrátíme se v algoritmu. Jinak proběhne rekurzivní volání, kdy je aktuální prvek {v} přidán k R, P a X jsou omezené na sousední uzly {v}. Tím se zajistí nalezení všech rozšířených klik R, které obsahují prvek {v}. Následně se přesune prvek {v} z P do X, čímž se vyloučí z dalších hledání. Algoritmus pokračuje dalším uzlem {v} z P.

