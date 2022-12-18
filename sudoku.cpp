// Name of program mainreturn.cpp
#include <iostream>
#include <cstring>
#include <string>
#include <fstream>



char **vytvorenie_sudoku(){
    //vytvorenie 2D poľa pre sudoku
    char **sudoku;
    sudoku = new char *[9];
    for (int i=0; i<9; i++)
        sudoku[i] = new char[9];
    return sudoku;
}

void vynulovanie(char **sudoku){
    for (int r=0; r<9; r++){
        for (int s=0; s<9; s++){
            sudoku[r][s]='0';
        }
    }
}

bool kontrola_stlpca(char **sudoku, int index_stlpca, char vkladane){
    for (int riadok=0; riadok<9; riadok++) {
        if (sudoku[riadok][index_stlpca]==vkladane){
            return false;
        }
    }
    return true;
}

bool kontrola_riadku(char **sudoku, int index_riadku, char vkladane){
    for (int stlpec=0; stlpec<9; stlpec++) {
        if(sudoku[index_riadku][stlpec]==vkladane){
            return false;
        }
    }
    return true;
}

bool kontrola_stvorca(char **sudoku, int index_riadku, int index_stlpca, char vkladane){
    int zaciatok_riadku = index_riadku - (index_riadku%3);
    int zaciatok_stlpca = index_stlpca - (index_stlpca%3);

    for (int riadok=0; riadok<3; riadok++) {
        for (int stlpec=0; stlpec<3; stlpec++){
            if (sudoku[riadok+zaciatok_riadku][stlpec+zaciatok_stlpca]==vkladane){
                return false;
            }
        }
    }
    return true;
}

bool je_bezpecne(char **sudoku, int riadok, int stlpec, char vkladane_cislo){
    //Kontrola políčok, či je možné tam vložiť číslo
    if (vkladane_cislo=='0' || vkladane_cislo=='.'){
        return true;
    }
    return (kontrola_riadku(sudoku,riadok,vkladane_cislo) && kontrola_stlpca(sudoku,stlpec,vkladane_cislo)
                      && kontrola_stvorca(sudoku,riadok,stlpec,vkladane_cislo));
}

int *najdi_prazdne(char **sudoku){
    int *prazdne;
    prazdne = new int [2];

    //V cykle sa prechádza sudoku, kým sa nenájde najbližšie voľné miesto
    for (int riadok=0; riadok<9; riadok++){
        for (int stlpec=0; stlpec<9; stlpec++){
            if (sudoku[riadok][stlpec]=='0' || sudoku[riadok][stlpec]=='.'){
                prazdne[0]=riadok;
                prazdne[1]=stlpec;
                return prazdne;
            }
        }
    }

    //Ak také nie je vráti pozíciu za koncom sudoku
    prazdne[0]=9;
    prazdne[1]=9;
    return prazdne;
}

bool riesenie_sudoku(char **sudoku){
    int riadok;
    int stlpec;
    int *prazdne;


    //Nájdu sa súradnice najbližšieho prázdneho políčka
    prazdne = najdi_prazdne(sudoku);
    riadok = prazdne[0];
    stlpec = prazdne[1];

    //Ak sú súradnice za koncom sudoku, je už vyriešené
    if (riadok==9 && stlpec==9){
        return true;
    }

    for (char cislo='1'; cislo<='9'; cislo++){

        //V cykle sa prejdú všetky možnosti čísel na zapísanie a ak sa nevyskytuje nikde v
        //stĺpci,riadku alebo štvorci, zapíše sa
        if (je_bezpecne(sudoku,riadok,stlpec,cislo)){
            sudoku[riadok][stlpec]=cislo;

            //Zapĺňaňnie pokračuje rekurzívne ďalej, kým sa nezaplní celé alebo nenastane zhoda niekde
            if (riesenie_sudoku(sudoku)){
                return true;
            }
        }

        //Ak nie, políčko sa naspäť vynuluje
        sudoku[riadok][stlpec] = '0';
    }
    //Ak sa nenájde žiadne také číslo, vráti sa program späť v rekurzii
    return false;
}

int zapisanie_sudoku(char **sudoku, char kde, const std::string& output){

    //Podľa znaku na vstupe sa určí, či sa zapisuje do konzoly alebo súboru
    if (kde=='k'){
        for (int r=0; r<9;r++){
            for (int s=0; s<9; s++){
                std::cout << sudoku[r][s];
            }
        }
        std::cout << std::endl;
    }


    else if (kde=='s'){
        std::ofstream subor(output);

        if (!subor.is_open()){
            return 4;
        }

        for (int r=0; r<9;r++){
            for (int s=0; s<9; s++){
                subor << sudoku[r][s];
            }
            subor << std::endl;
        }

        subor.close();
        if(subor.is_open()){
            return 6;
        }
    }
    return 0;
}

int nacitanie_subor(const std::string& input, const std::string& output, char kde){
    std::string riadok;
    std::ifstream subor(input);
    bool dobre_zadane = true;
    char **sudoku = vytvorenie_sudoku();

    if (!subor.is_open()){
        return 4;
    }

    long unsigned int aktualny_znak = 0;
    while (std::getline (subor,riadok)){

        //Ak sudoku nemá dĺžku 81, je zle zadané
        if (riadok.length()!=81){
            return 2;
        }

        for (int r=0; r<9; r++){
            for (int s=0; s<9; s++){

                //Aktuálny znak z reťzca na vstupe
                char znak = (riadok[aktualny_znak]);

                //V sudoku nie sú povelené iné znaky ako čísla a .
                if ((znak<'0' || znak>'9') && znak!='.'){
                    return 3;
                }

                //Ak sudoku je nesprávne zadané a nedá sa tým pádom vyriešiť, cyklus pre zapisovanie
                if(!je_bezpecne(sudoku,r,s,znak)){
                    dobre_zadane = false;
                    break;
                }

                sudoku[r][s] = znak;
                aktualny_znak+=1;
            }
        }

        //ak je sudoku dobre zadané a riešiteľné, rieši sa výstup inak vypíše prázdnyr riadok
        if (dobre_zadane && riesenie_sudoku(sudoku)) {
            zapisanie_sudoku(sudoku, kde, output);
        }
        else{
            std::cout << "\n";
        }

        //Nastavenie premien pre ďalší riadok/sudoku
        aktualny_znak = 0;
        dobre_zadane = true;
        vynulovanie(sudoku);
    }

    if (!subor.eof()){
        return 5;
    }

    subor.close();
    if(subor.is_open()){
        return 6;
    }

    return 0;
}

int nacitanie_konzola(const std::string& output, char kde){
    std::string riadok;
    char **sudoku = vytvorenie_sudoku();
    bool dobre_zadane = true;
    long unsigned int aktualny_znak = 0;


    while (std::getline(std::cin, riadok)){

        //Ak sudoku nemá dĺžku 81, je zle zadané
        if (riadok.length()!=81){
            return 2;
        }


        for (int r=0; r<9; r++){
            for (int s=0; s<9; s++){

                //Aktuálny znak z reťzca na vstupe
                char znak = (riadok[aktualny_znak]);

                //V sudoku nie sú povelené iné znaky ako čísla a .
                if ((znak<'0' || znak>'9') && znak!='.'){
                    return 3;
                }

                //Ak sudoku je nesprávne zadané a nedá sa tým pádom vyriešiť, cyklus pre zapisovanie
                if (!je_bezpecne(sudoku,r,s,znak)){
                    dobre_zadane = false;
                    break;
                }

                sudoku[r][s] = riadok[aktualny_znak];
                aktualny_znak += 1;
            }
        }

        //ak je sudoku dobre zadané a riešiteľné, rieši sa výstup inak vypíše prázdnyr riadok
        if (dobre_zadane && riesenie_sudoku(sudoku)) {
            zapisanie_sudoku(sudoku, kde, output);
        }
        else{
            std::cout << std::endl;
        }

        //Nastavenie premien pre ďalší riadok/sudoku
        dobre_zadane = true;
        aktualny_znak = 0;
        vynulovanie(sudoku);
    }
    return 0;
}

  
int main(int argc, char** argv)
{
    //načítanie sudoku z konzoly, zapíše sa do konzoly
    if (argc==1){
        return nacitanie_konzola("",'k');
    }

    else {
        //načítanie zo súboru, zapísanie do konzoly
        if (strcmp(argv[1],"-i")==0 && argc==3){
            return nacitanie_subor(argv[2], "", 'k');
        }

        //načítanie zo súboru, zapísanie do súboru
        else if (strcmp(argv[1],"-i")==0 && argc==5 && strcmp(argv[3],"-o")==0){
            return nacitanie_subor(argv[2],argv[4],'s');
        }

        //načítanie zo súboru, zapísanie do súboru
        else if (strcmp(argv[1],"-o")==0 && argc==5 && strcmp(argv[3],"-i")==0){
            return nacitanie_subor(argv[4],argv[2],'s');
        }

        //načítanie z konzoly, zapísanie do súboru
        else if (strcmp(argv[1],"-o")==0 && argc==3){
            return nacitanie_konzola(argv[1],'s');
        }

        //Zvyšné možnosti sú nesprávne zadané argumenty
        else{
            return 1;
        }
    }
}


/* Error kódy
 * 1 - zle zadané argumenty na vstupe
 * 2 - zlá dĺžka sudoku
 * 3 - nesprávne znaky v sudoku
 * 4 - chyba pri otváraní sudoku súboru
 * 5 - chyba v súbore
 * 6 - chyba pri zatváraní súboru
 */