#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstring>


//nacitanie suboru do vektora
std::vector<std::string> nacitanie_suboru(std::fstream &subor){
    std::vector<std::string> vektorovy_subor;
    std::string riadok;

    while(std::getline(subor,riadok)){

        //ak je ešte vstup, prida sa na koniec novy riadok
        if (subor.good()) {
            riadok += '\n';
        }
        vektorovy_subor.push_back(riadok);
    }

    subor.clear();
    return vektorovy_subor;
}

//overenie, ci string je cislo
bool kontrola_cisla(const std::string &cislo) {
    return !cislo.empty() && std::all_of(cislo.begin(), cislo.end(), ::isdigit);
}

//ziskanie prikazu a rozsahu zo vstupu
std::string analyza_vstupu(std::vector<std::string> &subor,std::string &vstup,int&riadok,int &zaciatok,int &koniec){
    //urcenie zaciatku a konca v pripade, ze nie je zadany rozsah -> vtedy to je od prveho riadku po koniec
    zaciatok = 1;
    koniec = subor.size();

    //ak je subor prazdny nastavi sa koniec na prvy riadok
    if(subor.empty()){
        koniec = 1;
    }
    riadok = subor.size();

    std::string prikaz, range;
    //aktualny parameter ukazuje ci sa hlada prikaz(1) alebo rozsah(2)
    int aktualny_parameter = 1;

    //citanie vstupu po slovach
    std::istringstream iss(vstup);
    while(iss){
        std::string substring;
        iss >> substring;

        //prve slovo sa nastavi na prikaz
        if(aktualny_parameter == 1){
            prikaz = substring;
            aktualny_parameter++;
        }

        //ak sa hlada range a este nie je vstup ukonceny, analyzuje sa druhe slovo, ci neobsahuje range
        else if(aktualny_parameter == 2 and strcmp(substring.c_str(),"\0") != 0){
            range = substring;
            //vyksyt , v stringu
            int vyskyt = std::count(range.begin(), range.end(), ',');

            //ak sa ciarka nevyskytuje a prikaz je append, nastavi sa to ako riadok
            if (vyskyt == 0 and kontrola_cisla(range) and prikaz[0] == 'a'){
                riadok = std::stoi(range);
            }

            else{
                //pripad, ked na vstupe je len jedno cislo
                if(vyskyt == 0 and kontrola_cisla(range)){
                    zaciatok = std::stoi(range);
                    koniec = std::stoi(range);
                }

                //pripad,ak je ciarka na zaciatku
                else if(range[0] == ',' and kontrola_cisla(range.substr(1,range.size()-1))){
                    koniec = std::stoi(range.substr(1,range.size()-1));
                }

                //pripad, ak je ciarka na konci
                else if(range[range.size()-1] == ',' and kontrola_cisla(range.substr(0,range.size()-1))){
                    zaciatok = std::stoi(range.substr(0,range.size()-1));
                }

                //ak sa ciarka vyskytuje v strede
                else if(vyskyt == 1){
                    int pozicia = range.find(',');
                    //urcenie zaciatku a konca a prevedenie ich na cisla
                    std::string prva_cast = range.substr(0, pozicia);
                    std::string druha_cast = range.substr(pozicia+1, range.length());

                    if(kontrola_cisla(prva_cast)){
                        zaciatok = std::stoi(prva_cast);
                    }

                    if(kontrola_cisla(druha_cast)){
                        koniec = std::stoi(druha_cast);
                    }

                    if(zaciatok > koniec){
                        prikaz = "invalid range";
                    }

                    if(prikaz[0] == 'a'){
                        prikaz = "";
                    }
                }

                //ak tam ciarka nie je, je to invalid range
                else if(strcmp(substring.c_str(),",")!=0){
                    prikaz = "invalid range";
                }

                //ak je range pri prikazoch, kde by nemal byt, prikaz nie je platny
                if(prikaz[0] == 'w' or prikaz[0] == 'q' or strcmp(substring.c_str(),"q!")==0){
                    prikaz = "";
                }
            }

            aktualny_parameter++;
        }

        else if(strcmp(substring.c_str(),"\0")!=0){
            prikaz = "";
        }

    }

    return prikaz;
}


bool quit(std::vector<std::string> &vektorovy_subor, std::vector<std::string> &meneny_subor, std::string &vstup){
    //program sa ukonci porovnanim vektorov pre stringy a podla toho ci si rovnake vypise upozornenei alebo nie
    if(strcmp(vstup.c_str(),"q") == 0){
        if(vektorovy_subor == meneny_subor){
            return true;
        }
        else{
            std::cout << "You have unsaved changes" << std::endl;
            return false;
        }
    }

    else if(strcmp(vstup.c_str(), "q!") == 0){
        return true;
    }

    else{
        return false;
    }
}


void write(std::fstream &subor,std::vector<std::string> &povodny_subor, std::vector<std::string> &meneny_subor,std::string &meno_suboru){
    //subor sa zatvori a otvori sa nanovo prazdny, aby sa mohol zapisat novy obsah
    subor.close();
    subor.open(meno_suboru, std::fstream::in | std::fstream::out | std::fstream::trunc);
    for(int i=0; i< static_cast<int>(meneny_subor.size());++i){
        subor << meneny_subor[i];
    }
    povodny_subor = meneny_subor;
}


void print(std::vector<std::string> &subor, int &zaciatok, int &koniec){
    if(koniec <= static_cast<int>(subor.size()) and zaciatok <= static_cast<int>(subor.size()) and zaciatok > 0 and koniec > 0) {
        for(int i = zaciatok-1 ; i < koniec; ++i) {
            std::cout << subor[i];
        }
    }
}


void delete1(std::vector<std::string> &subor, int &zaciatok, int &koniec){
    //v pripade, ze koniec je vacsi ako dlzka suboru, nastavi sa na koniec suboru
    if(koniec > static_cast<int>(subor.size()) and zaciatok <= static_cast<int>(subor.size())){
        koniec = subor.size();
    }

    //ak zaciatok je mimo rozsahu suboru ale je menej ako velkost suboru, nastavi sa na zaciatok
    if(zaciatok < 1 and zaciatok <= static_cast<int>(subor.size())){
        zaciatok = 1;
    }

    if(koniec <= static_cast<int>(subor.size()) and zaciatok <= static_cast<int>(subor.size()) and zaciatok > 0 and koniec > 0) {
        subor.erase(subor.begin() + zaciatok - 1, subor.begin() + koniec);
    }
}


void append(std::vector<std::string> &meneny_subor, int &riadok){
    //pridanie prazdnych riadkov, ak je riadok pridania vacsi ako velkost suboru
    while(riadok > static_cast<int>(meneny_subor.size())){
        std::string prvy;
        //ak na prvom slove chyba novy riadok, prida sa tam
        if(!meneny_subor.empty()){
            prvy = meneny_subor[0];
        }
        if(!prvy.empty() and prvy[prvy.length()-1] != '\n'){
            meneny_subor[0]+='\n';
        }
        meneny_subor.emplace_back("\n");
    }

    int dlzka_vstupu = 0;
    std::string novy_riadok;
    //nacitavanie vstupu pre append
    while(true){
        std::getline(std::cin, novy_riadok);
        std::string posledny_riadok;

        if(riadok>0) {
            posledny_riadok = meneny_subor[riadok - 1];
        }

        //ukonci sa ak je bodka na vstupe
        if(novy_riadok[0] == '.'){
            break;
        }

        //prida sa novy riadok
        if(std::cin.good()){
            novy_riadok += "\n";
        }

        //ak je riadok rovny velkosti suboru, pridava sa na koniec a nastavi sa nova velkost riadku
        int velkost = meneny_subor.size();
        if(riadok == velkost){
            if(!posledny_riadok.empty() and posledny_riadok[posledny_riadok.length()-1] != '\n') {
                meneny_subor.back() += "\n";
            }
            meneny_subor.push_back(novy_riadok);
            riadok++;
        }

        else{
            //ak nie prida sa na pozadovanu poziciu
            meneny_subor.insert(meneny_subor.begin()+riadok+dlzka_vstupu, novy_riadok);
            dlzka_vstupu++;
        }
    }
}


void change(std::vector<std::string> &subor, int &zaciatok, int &koniec){
    std::vector<std::string> novy_subor;
    std::string novy_riadok;

    //pridaju sa prazdne riadky na zaciatok, ak sa idu menit riadky za velkostou suboru
    while(zaciatok > static_cast<int>(subor.size()) or static_cast<int>(subor.size())==0){
        std::string prvy;
        if(!subor.empty()){
            prvy = subor[0];
        }
        if(!prvy.empty() and prvy[prvy.length()-1] != '\n'){
            subor[0]+='\n';
        }
        subor.emplace_back("\n");
    }

    //nacitanie noveho vstupu, kym nebude .
    while(true){
        std::getline(std::cin, novy_riadok);

        if(novy_riadok[0] == '.'){
            break;
        }

        if(std::cin.good()){
            novy_riadok += "\n";
        }

        novy_subor.push_back(novy_riadok);
    }

    //ak je zadany cely rozsah, nahradi sa subor
    if((zaciatok == 1 and koniec == static_cast<int>(subor.size())) or (zaciatok == 1 and koniec == 1)){
        subor = novy_subor;
    }

    else{
        //inak sa pozadovane riadky vymazu a nahradia sa novym vstupom
        int pozicia = zaciatok-1;
        delete1(subor, zaciatok, koniec);
        for(int i=0; i<static_cast<int>(novy_subor.size()); i++){
            subor.insert(subor.begin()+pozicia,novy_subor[i]);
            pozicia++;
        }
    }

}


void textovy_editor(std::fstream &subor, std::string &meno_suboru){
    std::vector<std::string> vektorovy_subor = nacitanie_suboru(subor);
    std::vector<std::string> zmeneny_subor = vektorovy_subor;
    std::string vstup, prikaz;
    int riadok, zaciatok, koniec;

    //volanie funkcii podla zadaneho prikazu a rozahu na vstupe
    while(true){
        std::cout << "* ";
        std::getline(std::cin, vstup);
        prikaz = analyza_vstupu(zmeneny_subor,vstup,riadok,zaciatok,koniec);

        if(strcmp(prikaz.c_str(),"q")==0 or strcmp(vstup.c_str(),"q!")==0){
            if(quit(vektorovy_subor,zmeneny_subor,vstup)){
                break;
            }
        }

        else if(strcmp(prikaz.c_str(),"w")==0){
            write(subor,vektorovy_subor,zmeneny_subor, meno_suboru);
        }

        else if(strcmp(prikaz.c_str(),"d")==0){
            delete1(zmeneny_subor,zaciatok,koniec);
        }

        else if(strcmp(prikaz.c_str(),"a")==0){
            append(zmeneny_subor, riadok);
        }

        else if(strcmp(prikaz.c_str(),"p")==0){
            print(zmeneny_subor,zaciatok,koniec);
        }

        else if(strcmp(prikaz.c_str(),"c")==0){
            change(zmeneny_subor,zaciatok,koniec);
        }

        //ak sa prikaz s nicim nezhoduje, nie je podporovany
        else{
            if(strcmp(prikaz.c_str(),"invalid range")==0){
                std::cout << "Invalid range" << std::endl;
            }
            else {
                std::cout << "Unsupported command" << std::endl;
            }
        }
    }

    subor.close();
    if(subor.is_open()){
        exit(3);
    }
}


int main(int argc, char** argv) {
    std::string meno_suboru;
    if (argc == 2) {
        meno_suboru = argv[1];
        std::fstream subor;
        subor.open(meno_suboru, std::fstream::in | std::fstream::out);

        if (!subor.is_open()) {
            return 2;
        }

        textovy_editor(subor,meno_suboru);
    }
    else if(argc==1){
        std::fstream subor;
        meno_suboru = "vystup2.txt";
        subor.open(meno_suboru, std::fstream::in | std::fstream::out | std::fstream::trunc);

        if(!subor.is_open()){
            return 2;
        }

        textovy_editor(subor,meno_suboru);
    }
    else {
        return 1;
    }

    return 0;
}


/*
 * chybove hlasenia
 * 1 - nespravny pocet argumentov
 * 2 - chyba pri otvoreni suboru
 * 3 - chyba zatvorenia suboru
 */

