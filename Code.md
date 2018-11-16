## Iniziamo dagli argomenti
**Usage: ./MappeIrpi DATE1 DATE2 CONFIGFILE  -v SPEED -f FORMAT -m MAPS**  
DATE1: La data dalla quale si vogliono iniziare a generare i file  
DATE2: La data fino alla quale quale si vogliono generare i file  
SPEED: i millisecondi tra un frame e l'altro  
FORMAT: formato di output, può essere .gif o .mp4 (scrivere anche il punto)  
MAPS: nomi delle mappe che si vogliono elaborare (es: c_3 c_6 cf_psm)  
  
  
## Il codice
```    
//init vips and register gdal driver
if (VIPS_INIT(argv[0]))
    vips_error_exit(NULL);
GDALAllRegister();
// Disable te libvips cache -- it won't help and will just burn memory.
vips_cache_set_max(0);
```
Inizializza libvips e gdal, cioè carica tutti i driver gdal e dice a libvips dove lavorare

---
Inizializza alcune variabili necessarie, legge alcuni file necessari come lo sfondo o le zone d'allerta e crea un vettore contenente i nomi delle mappe partendo dagli argomenti con i quali è stato richiamato il programma
Passa al costruttore della classe che gestisce le preferenze il nome del file dal quale leggere le impostazioni  
È un file di testo semplice facilmente leggibile  
Le date vengono convertite in una struttura dati chiamata Tm perché per questa esistono tutte le funzioni che permettono la manipolazione del tempo, conversione in stringhe ecc..  
A gdal viene detto di usare il canale alpha
```
//load conf from the configuration file
preferences conf(argv[3]);

//takes the args and convert them to tm struct
tm startDate = toTmStruct(std::stringstream(argv[1]), conf.getDIR_FORMAT());
tm endDate = toTmStruct(std::stringstream(argv[2]), conf.getDIR_FORMAT());
int diffHours = (int) std::difftime(timegm(&endDate), timegm(&startDate)) / 3600;

GDALDataset *originalDataset;
GDALDataset *tempDataset;
//option for the apply color to the tif file and set the alpha
char *optionForDEM[] = {const_cast<char *>("-alpha"), nullptr};
GDALDEMProcessingOptions *options = GDALDEMProcessingOptionsNew(optionForDEM, nullptr);

//read background and overlay
VImage background = VImage::new_from_file(conf.getBACKGROUND());
VImage overlay = VImage::new_from_file(conf.getOVERLAY());
//    VImage marker = vips::VImage::new_from_file(conf.getMARKER());

std::vector<std::string> mapNames;
for (int i = 9; i < argc; ++i) {
    mapNames.push_back(std::string(argv[i]));
}
```
---
Crea la cartella dove andranno a finire i file temporanei e gli output dell'attuale lavorazione
```
// The path where goes all the files for the current elaboration
std::string elaborationDir(argv[1]);
elaborationDir.append(argv[2]);
fs::create_directory(fs::path(conf.getTEMP_PATH() + elaborationDir));
```
---
Variabili che servono per districarsi tra le cartelle e per trovare i file giusti
```
// The dir where goes the current day/map files
std::string currentNewFilesDir;
// The where take from the original TIFs
char currentOriginalDatasetDayDir[12];
// The buffer containing the timestamp
char timestampString[23];
// The frame currently in working
time_t actualTime;
// The return code of the gdal work. Debug Only
int gdalReturnCode;
// The name of the file containing the color scale, changes each cycle.
std::string colorFile;
```
---
Inizia il ciclo principale  
Cicla con un for : each ogni tipo di mappa che è stato richiesto
```
    for (auto &map : mapNames) {
```
---
Resetta la data dalla quale iniziare a cercare immagini a quella passata come argomento  
Crea una sottocartella per la mappa attuale  
Assegna alla variabile colorFile il nome del file della scala colore necessario al ciclo attuale
```
// reset the startDate for next day
startDate = toTmStruct(std::stringstream(argv[1]), conf.getDIR_FORMAT());
currentNewFilesDir = conf.getTEMP_PATH() + elaborationDir + "/" + map;
fs::create_directory(fs::path(currentNewFilesDir));

// if the last character of the name is a number, to find the corresponding color file I have to delete it
colorFile = map;
while (isdigit(*colorFile.rbegin())) {
    colorFile.pop_back();
}
colorFile.append("-colors.txt");
```
---
Cicla la mappa attuale per ogni ora che è stata chiesta e aggiorna il tempo della foto in elaborazione (in fondo al ciclo viene incrementata, ma qui viene ricalcolata con il nuovo valore)
```
for (int ora = 0; ora < diffHours; ++ora) {
    actualTime = timegm(&startDate);
    strftime(currentOriginalDatasetDayDir, 12, conf.getDIR_FORMAT(), gmtime(&actualTime));
```
---
Utilizzando gdal, apre il tif originale in lettura, lo elabora e il risultato viene messo nella variabile tempdataset, che poi viene scritta su disco
```
originalDataset = (GDALDataset *) GDALOpen(
        (conf.getBASE_PATH() + currentOriginalDatasetDayDir + "/" + map + ".tif").c_str(), GA_ReadOnly);
tempDataset = (GDALDataset *) GDALDEMProcessing(
        (currentNewFilesDir + "/" + std::to_string(ora) + map + ".tif").c_str(),
        originalDataset, "color-relief",
        colorFile.c_str(), options, &gdalReturnCode);
GDALClose(tempDataset); //write the processed gdalTif to disk
```
---
Con Vips apre la nuova foto elaborata da gdal  
La ridimenziona fino a raggiungere la dimenzione dello sfondo  
Aggiunge lo sfondo all'immagine appena ridimenzionata
Dopodiche aggiunge anche le zone a rischio  
Il modo in cui lo fà è frutto di vari tentativi, comunque, aprire un immagine in access sequential è il modo più veloce di elaborare per vips  
VIPS_KERNEL_NEAREST è il metodo di intepolazione, ovvero nearest (sdoppia i pixel senza creare sfumature o alterare i dati)  
Detto ciò, prosegue creano la stringa del timestamp e sovrappone anche questa al frame in elaborazione
```
VImage tif = vips::VImage::new_from_file(
        (currentNewFilesDir + "/" + std::to_string(ora) + map + ".tif").c_str(),
        VImage::option()->set("access", "sequential"));
tif = tif.resize(3.022222222, VImage::option()->set("kernel", VIPS_KERNEL_NEAREST));
VImage frame = background.composite2(tif, VIPS_BLEND_MODE_OVER);
frame = draw_overlay(frame, overlay, 0, 0, {0});

/* we must make a unique string each time, or libvips will
    * cache the result for us
    */
strftime(timestampString, 22, conf.getDATE_FORMAT(), gmtime(&actualTime));
VImage text = VImage::text(timestampString,
                            VImage::option()->set("font", "SFmono 35")->set("fontfile", conf.getFONT()));
frame = draw_overlay(frame, text, 470, 10);
```
---
Scrive su disco la foto e rimuove il tif creato da gdal
```
frame.write_to_file((currentNewFilesDir + "/" + std::to_string(ora) + ".jpeg").c_str());
remove((currentNewFilesDir + "/" + std::to_string(ora) + map + ".tif").c_str());
```
---
Invoca lo script bash corrispondente al formato richiesto  
La cosa non è molto elegante, anzi, è più un workaound. Questo però semplifica molto l'impostazione della cartella di lavoro, e anche se ho provato di farne a meno alla fine solo così mi funziona  
Controlla l'argomento corrispondente al formato e passa allo scrit la cartella di lavoro, la velovità e il nome dell'output
```
std::string command;
//controlla l'argomento del formato
if (strcmp(argv[7], ".mp4") == 0) {
    command = "sh mp4.sh " + currentNewFilesDir + " " + argv[5] + " " + "output";
    system(command.c_str());
} else {
    command = "sh gif.sh " + currentNewFilesDir + " " + argv[5] + " " + "output";
    system(command.c_str());
}
```
---
Converte una stringa correttamente formattata in una Struct tm
```
tm toTmStruct(std::stringstream dateTime, std::string DIR_FORMAT) {
    struct tm newTm{};
    dateTime >> std::get_time(&newTm, DIR_FORMAT.c_str());
    return newTm;
}
```
---
Converte una coppia di coordinate dal sistema di riferimenti 4326 al 3857, usando l'utility di gdal preposta
```
void to3857(double *x, double *y) {
    OGRSpatialReference sourceSRS, targetSRS;
    sourceSRS.importFromEPSG(4326);
    targetSRS.importFromEPSG(3857);
    OGRCreateCoordinateTransformation(&sourceSRS, &targetSRS)->Transform(1, x, y);
}
```
---
Sovrappone due immagini  
Questa è la funzione apposita per aggiungere l'overlay, infatti usa la sottrazione dallo sfondo e il riempimento con l'overlay solo nelle zone sottratte per migliorare l'efficienza
```
static VImage
draw_overlay(VImage background, VImage overlay, int x, int y, std::vector<double> ink = {255}) {
    // cut out the part of the background we modify
    VImage tile = background.extract_area(x, y, overlay.width(), overlay.height());

    // Make a constant image the size of the overlay area containing the ink
    VImage ink_image = overlay.new_from_image(ink);

    // use the overlay as a mask to blend smoothly between the background and the ink
    tile = overlay.ifthenelse(ink, tile, VImage::option()->set("blend", TRUE));

    // and insert the modified tile back into the image
    return background.insert(tile, x, y);
}
```
---
Sovrappone due immagini  
Questa è la funzione apposita per aggiungere un immagine piccola ad una più grande
```
static VImage
composite_glyph(VImage background, VImage glyph, int x, int y) {
    // cut out the part of the background we modify
    VImage tile = background.extract_area(x, y, glyph.width(), glyph.height());

    // use the overlay as a mask to blend smoothly between the background
    // and the ink
    tile = tile.composite(glyph, VIPS_BLEND_MODE_OVER);

    // and insert the modified tile back into the image
    return background.insert(tile, x, y);
}
```
---