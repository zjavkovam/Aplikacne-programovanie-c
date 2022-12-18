#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <cstring>



//premena argumentu z char* na cislo
uint64_t konverzia_parametra_n (const std::string &string_n){
    uint64_t n = 0;
    uint64_t cislica;

    if((string_n[0]=='0' and string_n.length()==1) or string_n[0]=='-') {
        exit(2);
    }

    for(long unsigned int i = 0; i<string_n.length(); i++) {
        if(string_n[i]<'0' or string_n[i]>'9'){
            exit(2);
        }
        cislica = static_cast<uint64_t>(string_n[i])-48;
        n+=cislica*static_cast<uint64_t>(pow(10,string_n.length() - i-1));
    }

    //ak by bol vacsi ako uint32_t vrati chybu
    if(n>4294967295U){
        exit(2);
    }
    return n;
}

long unsigned int vypis_substringy(const std::string &povodny_retazec, const std::string &hladany_retazec, uint32_t &vzdialenost,const uint32_t &n, uint64_t &riadok,
                     uint64_t &pozicia_v_riadku,std::pair <uint64_t ,uint64_t> &posledny_vyskyt,std::string &predchazdajuci_buffer,
                     std::pair <uint64_t,uint64_t> &zaciatok, std::pair <uint64_t ,uint64_t> &posledne_vypisane, bool &naslo_predchadzajuci){

    long unsigned int najdene = 0;
    std::string porovnavaci_retazec,retazec;

    //pripojenie predchadzajuceho buffera na zaciatok aktualneho
    retazec = predchazdajuci_buffer + povodny_retazec;

    for(long unsigned int i=0; i <retazec.length(); i++){
        char znak = retazec[i];

        //ak sa aktualny znak zhoduje s prvym v slove, precita sa substring dlzky hladaneho slova z retazca a porovna sa, ci sa
        //cele slovo zhoduje
        if(znak == hladany_retazec[0]){
            porovnavaci_retazec = retazec.substr(i,hladany_retazec.length());

            if(strcmp(hladany_retazec.c_str(),porovnavaci_retazec.c_str())==0){

                //podmienky o vypisoch, ci uz bolo vypisane a je v pozadovaneh vzdialenosti...
                if(naslo_predchadzajuci and vzdialenost < n and (posledny_vyskyt.first!=riadok or posledny_vyskyt.second != pozicia_v_riadku)){
                    if(posledne_vypisane != posledny_vyskyt) {
                        std::cout << posledny_vyskyt.first << " " << posledny_vyskyt.second << std::endl;
                    }
                    std::cout << riadok << " " << pozicia_v_riadku << std::endl;
                    posledne_vypisane.first = riadok;
                    posledne_vypisane.second = pozicia_v_riadku;
                }

                //ak aj netreba vypisat, nastavi sa ako posledne najdene.
                vzdialenost = 0;
                posledny_vyskyt.first = riadok;
                posledny_vyskyt.second = pozicia_v_riadku;
                naslo_predchadzajuci=true;


                //Pre prípad prelínania, sa odpočítava vzdialenost a nastavi sa novy zaciatok podla toho ci tam je novy riadok
                if(najdene>0){
                    vzdialenost--;
                    if (hladany_retazec[najdene] == '\n') {
                        zaciatok.first += 1;
                    }
                    else {
                        zaciatok.second += 1;
                    }
                }
            }

            else if(vzdialenost <= 4294967295U){
                vzdialenost++;
            }
        }
        else if(vzdialenost <= 4294967295U){
            vzdialenost++;
        }

        //pocitanie kolko naslo znakov z hladaneho retazca a nastavenie zaciatku
        if(znak==hladany_retazec[najdene]){
            if(najdene==0){
                zaciatok.first = riadok;
                zaciatok.second = pozicia_v_riadku;
            }

            if(najdene<hladany_retazec.length()-1) {
                najdene++;
            }
        }
        else{
            najdene = 0;
        }


        //aktualizacia riadku a pozicie v riadku
        pozicia_v_riadku++;
        if (znak == '\n') {
            riadok++;
            pozicia_v_riadku = 0;
        }
    }
    return najdene;
}

int main(int argc, char** argv) {
    if(argc==4) {
        std::string nazov_suboru = argv[1];
        std::ifstream subor(nazov_suboru);

        if (!subor.is_open()){
            return 4;
        }

        //kontrola dlzky retazca X
        std::string hladany_retazec = argv[2];
        if(hladany_retazec.length()>256 or hladany_retazec.length() <= 0){
            return 3;
        }

        //kontrola, ci v retazci nie su nepovolene znaky
        for(char znak : hladany_retazec){
            if(znak < 0 or znak > 127) {
                return 4;
            }
        }

        //vzdialenost N
        std::string string_n = argv[3];
        uint32_t n = konverzia_parametra_n(string_n);



        bool naslo_predchadzajuci = false;
        int velkost_bloku = 1024;
        uint32_t vzdialenost = 0;
        uint64_t riadok=0, pozicia_v_riadku = 0, najdene = 0;
        std::pair <uint64_t ,uint64_t> posledny_vyskyt(-1,-1), zaciatok(-1,-1), posledne_vypisane(-1,-1);
        std::string predchadzajuci_buffer, aktualny_rezazec;


        while(!subor.eof()){
            //program cita subor do konca a nacitava postupne bloky textu do aktualneho retazca
            //velkost bloku je nastavena na 1024
            aktualny_rezazec.clear();
            aktualny_rezazec.resize(velkost_bloku);
            subor.read(aktualny_rezazec.data(), aktualny_rezazec.size());

            //skontroluje sa ci, nie je tam nepovoleny znak, ak ano vyhodi error
            for(char znak : aktualny_rezazec){
                if(znak < 0 or znak > 127 ) {
                    return 3;
                }
            }


            //riesenie predchadzajuceho bufferu, kontoluje sa iba vtedy, ak naslo na konci znak(y) z hladaneho retazca
            if(najdene>0){

                //ziska poslednu a prvu cast retazca, ktora sa zhoduje s hladanym ak dokopy daju spravne slovo prerpise sa predchadzajuci buffer
                //ak tam nie je zhoda,predchadzajuci buffer sa resetuje
                std::string posledne = predchadzajuci_buffer.substr(predchadzajuci_buffer.length()-najdene,najdene);
                std::string prve = aktualny_rezazec.substr(0,hladany_retazec.length()-najdene);
                std::string oboje = posledne+prve;


                if(strcmp(oboje.c_str(),hladany_retazec.c_str())==0){
                    predchadzajuci_buffer.clear();
                    predchadzajuci_buffer.resize(posledne.length());
                    predchadzajuci_buffer = posledne;

                    //vrati sa spat na poziciu, kde slovo zacinalo a posunie sa vzdialenost
                    riadok = zaciatok.first;
                    pozicia_v_riadku = zaciatok.second;
                    if(vzdialenost>posledne.length()) {
                        vzdialenost -= posledne.length();
                    }
                }

                else{
                    predchadzajuci_buffer.clear();
                }
            }
            else{
                predchadzajuci_buffer.clear();
            }


            //funkcia na najdenie dvojici
            najdene = vypis_substringy(aktualny_rezazec,hladany_retazec,vzdialenost,n,riadok,pozicia_v_riadku, posledny_vyskyt,predchadzajuci_buffer,zaciatok,
                                       posledne_vypisane,naslo_predchadzajuci);

            //zapamatanie predchadzajuceho konca casti
            predchadzajuci_buffer.resize(hladany_retazec.length());
            predchadzajuci_buffer.clear();
            predchadzajuci_buffer = aktualny_rezazec.substr(velkost_bloku-hladany_retazec.length(),hladany_retazec.length());
        }

        return 0;
    }
    return 1;
}


/* Error kody
 * 1 - zly pocet argumentov
 * 2 - zle zadané n
 * 3 - zle zadaný retazec na hladanie
 * 4 - výskyt znaku, ktoý nie je v rozmedzí od 0 do 127

 */