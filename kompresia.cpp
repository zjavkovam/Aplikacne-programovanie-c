#include <iostream>
#include <fstream>


bool kontrola_pismena(const char &znak){
    if((znak >= 'a' and znak <= 'z') or (znak >= 'A' and znak <= 'Z')){
        return true;
    }
    return false;
}


bool kontrola_cisla(const char &znak){
    if(znak >= '0' and znak <= '9'){
        return true;
    }
    return false;
}


void kompresia(std::ifstream& vstup, std::ofstream& vystup){
    std::string riadok;
    int cislo_riadku = 1, pocet_aktualneho_znaku = 0;

    while (!vstup.eof()){
        std::getline(vstup, riadok);
        char aktualny_znak = riadok[0];

        //prechádza riadok po znakoch
        for (char znak : riadok){
            if(kontrola_pismena(znak)){

                //ak je znak v retazci rovnaký ako práve zapisovaný znak, ta ksa zvýši počet
                if(aktualny_znak == znak){
                    pocet_aktualneho_znaku++;
                }
                else{
                    //inak sa zapíše ako nový znak na počítanie a pôvodný sa zapíše do súboru
                    vystup << aktualny_znak << pocet_aktualneho_znaku;
                    aktualny_znak = znak;
                    pocet_aktualneho_znaku = 1;
                }
            }
            else{
                std::cout << "Error on line " << cislo_riadku << std::endl;
                exit(4);
            }
        }
        //zapísanie posledného znaku na konci riadku
        if(aktualny_znak != '\0') {
            vystup << aktualny_znak << pocet_aktualneho_znaku;
            pocet_aktualneho_znaku = 0;
        }
        if(!vstup.eof()){
            vystup << std::endl;
        }
        cislo_riadku++;
    }
}


void dekompresia(std::ifstream& vstup, std::ofstream& vystup){
    std::string riadok;
    int cislo_riadku = 1;
    while(!vstup.eof()){
        std::getline(vstup, riadok);

        //ak je riadok prádzny, pokračuje sa na ďalší
        if(riadok.length() == 0){
            if(!vstup.eof()) {
                vystup << std::endl;
            }
            cislo_riadku++;
            continue;
        }

        char aktualny_znak = '\0';
        int pocet = 0, index = 0;
        while(index < static_cast<int>(riadok.length())){
            //ak nie je zapísaný žiadny znak, zapíše sa ten čo je na v stupe v prípade, že je to písmeno
            if(kontrola_pismena(riadok[index]) and aktualny_znak == '\0'){
                aktualny_znak = riadok[index];
                index++;
            }
            //ak už je zadaný znak a znak na vstupe je číslo, pripočíta sa to k počtu
            else if(kontrola_cisla(riadok[index]) and index!=0){
                pocet = pocet*10 + (riadok[index]-'0');
                index++;
            }

            else{
                //ak to nie je ani jedno a pocet je 0, je to chyba
                if(pocet == 0) {
                    std::cout << "Error on line " << cislo_riadku << std::endl;
                    exit(4);
                }
                //ak je oboje správne zadane, zapise sa do suboru
                else{
                    for(int i=0; i < pocet; i++){
                        vystup << aktualny_znak;
                    }
                    pocet = 0;
                    aktualny_znak = '\0';
                }
            }
        }

        if(pocet == 0) {
            std::cout << "Error on line " << cislo_riadku << std::endl;
        }
        else{
            for(int i=0; i < pocet; i++){
                vystup << aktualny_znak;
            }
        }

        if(!vstup.eof() and riadok.length()>0){
            vystup << std::endl;
        }
        cislo_riadku++;
    }
}


int main(int argc, char** argv) {
    if(argc == 4){
        std::string co_spravit = argv[1];
        std::string nazov_suboru_vstup = argv[2], nazov_suboru_vystup = argv[3];
        std::ifstream vstup(nazov_suboru_vstup);
        std::ofstream vystup(nazov_suboru_vystup);

        if (!vstup.is_open() or !vystup.is_open()){
            return 3;
        }

        if(co_spravit == "compress"){
            std::string riadok;
            kompresia(vstup, vystup);
        }
        else if(co_spravit == "decompress"){
            dekompresia(vstup, vystup);
        }
        else{
            return 2;
        }

        vstup.close();
        vystup.close();
    }
    else{
        return 1;
    }
    return 0;
}

/*
 * 1 - zly pocet argumentov
 * 2 - zle zadane parametre
 * 3 - nepodarilo sa otvorit súbory
 * 4 - chyba v subore
 */
