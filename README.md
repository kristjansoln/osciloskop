# KRMILNIK NAPETOSTI KONDENZATORJA
ZAKLJUČNI PROJEKT PRI PREDMETU OSNOVE MIKROPROCESORSKE ELEKTRONIKE  
Kristjan Šoln  
Ljubljana, avgust 2021  
  
Video delovanja: https://drive.google.com/file/d/1-J9S_o9b7R6E5liVCJ3NMwhoj98kqPy8/view  
### 1. OPIS IDEJE  
Prvotna ideja tega projekta je »chopper« Miško gonilnik za koračni motor. »Chopper drive« je eden izmed najboljših načinov krmiljenja koračnega motorja, saj lahko dosežemo zelo dobro navorno-hitrostno karakteristiko. Tuljave motorja priklopimo na visokonapetostni vir za doseganje hitrih tokovnih sprememb in izvajamo sekanje toka, torej se izvede le prvi del tokovnega prehodnega pojava na navitju. S tem dosežemo skoraj idealne oblike tokovnih pulzov na navitju, kot kaže slika 1, in se izognemo situaciji na sliki 2.  
Ideja prvotnega projekta je bila, da bi preko napetosti na zaporednem uporu sklepal na tok navitja, in ustrezno preklapljal tranzistor, da bi končen tok imel obliko grafa zgoraj. Zdaj projekt nima te funkcionalnosti, pač pa krmili napetost na RC členu po podobnem principu. Izdelal sem torej neke vrste podlago za končno funkcionalnost: merjenje napetosti na več ADC kanalih ter krmiljenje vhodov na podlagi meritev. Za model tuljave motorja sem uporabil RC člen in meril njegov prehodni pojav. Ker doma nimam osciloskopa, sem dodal tudi izris krivulj na LCD. Iz teh razlogov je bilo ime tega projekta najprej »Osciloskop«, isto velja za imena datotek v projektu.  
Če povzamem, je ta projekt mišljen kot osnova oz. podlaga za razvoj »chopper« gonilnika koračnih motorjev, za demonstracijo pa uporablja prehodni pojav RC vezja. Več o možni nadgradnji je opisano v zaključku.  
  
Slika 1:  
![Slika 1](https://i.ibb.co/mvY7Qpd/Slika1.png)  
  
Slika 2:  
![Slika 2](https://i.ibb.co/tKKnJwv/Slika2.png)  

### 2. PRINCIP DELOVANJA
Vezje na sliki 3 je uporabljeno kot model tuljave koračnega motorja, kjer namesto toka tuljave krmilimo napetost kondenzatorja. Prikazano je na sliki desno. Označeni sta obe mesti priklopa na mikrokrmilnik. ADC kanal je povezan na zgornjo sponko kondenzatorja z oznako »READ_FROM«. Ko je sponka »WRITE_TO« v nizkem stanju, se kondenzator polni, ko pa je v visokem stanju, se le-ta prazni. Miško hkrati krmili 4 taka vezja.  
Sledi opis programa. Najprej se inicializirajo vhodi in izhodi. Sledi inicializacija AD pretvornika. Pripravljene so različne nastavitve preddelilnikov, ki omogočajo nastavljanje frekvence od 500 kHz do 4 MHz. Uporabljen je pomik levo za podatkovna registra, saj je uporabljena 8-bitna resolucija. Prioriteta je namreč hitrost, ne natančnost. Vklopljeno je proženje prekinitev, ročno proženje začetka konverzije in AVCC kot referenca.  
Nato se izriše odzadje. To pomeni izris osi grafa, oznake mej za preklop (sive črte na grafu) ter ostale oznake.  
Za tem pride program v neskončno zanko. Najprej v »while« zanki počaka na stisk tipke. Tipka A prikaže legendo ter informacije o kanalih, torej imena pinov, barve grafa ter zvezdico, če je kanal trenutno v uporabi. Tipka ESC pobriše graf, torej še enkrat nariše odzadje.  Tipka OK sproži konverzijo in nadaljevanje programa.  
Najprej se izvede 1. meritev, katere rezultat se zavrže zaradi nenatančnosti. Vmes se za kratek čas izklopijo prekinitve. To je funkcija osc_ADC_Read_by_pooling(). Nato se s klicem osc_ADC_start_conversion() prične veriga konverzij, kjer se na vsakem kanalu posebej izvede 100 meritev, ki se shranijo v svoje bufferje. Glej nadaljevanje.  
Dogajanje se preseli v prekinitveno rutino ADC pretvornika, ki se nahaja v knjižnici. Kliče se ob vsaki končani konverziji. Najprej shrani prebrano vrednost. Poskusi jo shranit v ustrezen buffer. Vsak kanal ima svoj buffer z dolžino 100 bitov, torej za 100 vrednosti.  Če je shranjevanje neuspešno, so bufferji polni in se veriga konverzij konča. Če pa je uspešno, se nastavi naslednji kanal, zažene nova konverzija in izvede krmiljenje izhodov glede na aktualno prebrano vrednost. Ta preveri aktualno prebrano vrednost. Če ta ni znotraj mej, se vklopi polnjenje ali praznjenje kondenzatorja.  
Po koncu konverzij se dogajanje spet vrne v main() funkcijo, kjer program skoči iz »while(wait)« zanke. Sledi izpis na ekran. Za vsak kanal posebej se izvede izris krivulje na graf. Vrednosti za izris bere iz ustreznega bufferja in riše pike na ustreznih mestih na ekranu. Vsak kanal ima svojo prednastavljeno barvo, da se jih loči med sabo.  
  
Slika 3:  
![Slika 3](https://i.ibb.co/LnVYkX6/Slika3.png)  
  
### 3. KOMENTAR NA DELOVANJE
Glavne omejitve je tukaj predstavljal čas izvajanja in dolžina ISR rutine. Končan projekt komajda zadostuje zastavljenim časovnim okvirom. Zmanjšal sem resolucijo in povečal frekvenco AD pretvornika. Prav tako sem vsepovsod, če se je dalo, uporabljal 8-bitne spremenljivke. Ena konverzija traja 14 ADC ciklov, kar znaša pri trenutnih nastavitvah 112 ciklov ure oz. 7 µs. Ko se kliče ISR rutina, shranjevanje registrov na sklad traja 32 ciklov, shranjevanje podatka v buffer 51 ciklov, inkrementiranje kanala 20 ciklov, ostale stvari do klica začetka naslednje konverzije pa 26 ciklov. To je skupaj 161 ciklov za ISR, kar je okrog 10 µs. Med dvema konverzijama mine torej 17 µs. Ker so uporabljeni štirje kanali, mine do naslednjega branja istega kanala štirikrat toliko, torej 68 µs.  
Ob začetku projekta sem predpostavil časovno konstanto navitij motorja okrog 0,5 ms. Želel sem vsaj 10 meritev znotraj te časovne konstante, kar bi pomenilo največ 50 µs med dvema meritvama na kanalu. Dobljenih 68 µs je izven teh okvirjev, vendar sem z rezultatom še vedno zadovoljen.  
Zaželjena je seveda čim višja frekvenca AD pretvornika ob dovolj natančnih meritvah. Izkazalo se je, da je frekvenca 2 MHz najbolj primerna, ker pri 4 MHz krivulja postane opazno »žagasta«.  
Za ročno proženje konverzije sem se odločil zaradi lažjega preklapljanja med kanali in določanja, kater podatek spada v kateri kanal. Samodejno proženje bi zapletlo zadeve, prav tako proženje s pomočjo časovnikov. »Pooling« pa je seveda najmanj optimalna izbira iz vidika časovne optimizacije.  
Vso izrisovanje na LCD je nastavljivo po Y osi. To se nastavi v makroju #define OSC_LCD_Y_SCALE_FACTOR. Funkcija zna tudi pobarvati površino pod grafom, vendar se je to izkazalo kot nepotrebno. Širina stolpcev je fiksna in znaša 1 piksel, dolžina časovne osi pa je nastavljiva z dolžino bufferja.  
S pomočjo makrojev in deklaracij bufferjev se določi tudi, kateri kanali naj se uporabljajo.  
### 4. ZAKLJUČEK
Projekt se mi zdi kot dobra podlaga za nadgradnjo na dejanski krmilnik koračnega motorja. Pri pisanju sem se poskušal držati delitve na strojni, sistemski in aplikacijski del. Sklepal sem, da delovanje AD pretvornika sodi pod strojni del, krmiljenje pod sistemski del, vse v povezavi z LCD pa v aplikacijski del.  
Če bi projekt nadgradil v gonilnik za motor, bi struktura programa zgledala malo drugače. Pretvarjanje se ne bi končalo po 100 meritvah kanala, tako kot se zdaj, ampak bi se nadaljevalo v neskončno. AD pretvornik bi še vedno imel svojo ISR rutino, kjer bi podatke shranjeval v buffer. Vpeljal bi časovnik, ki bi v svoji prekinitveni rutini skrbel za inkrementiranje »koraka« motorja in to shranjeval v globalne spremenljivke. Na podlagi teh spremenljivk se bi v main() funkciji izvajalo branje bufferjev in krmiljenje izhodov.  
Mogoče je vredno premisliti tudi, če je buffer res nujno potreben. Časovno je zelo potraten, če bi nekako lahko delil eno ali dve spremenljivki med main() funkcijo in ISR rutino AD pretvornika, bi lahko prihranil do 50 ciklov pri branju vhodov.  
