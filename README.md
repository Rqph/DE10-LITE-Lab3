# DEMAGALHAES_Lab3

# Hardware Description
## Architecture
On souhaite dans ce lab implémenter sur la carte DE10-lite le système suivant, qui affiche sur les afficheurs 7 segments l'accélération mesuré par l'accéléromètre (ADXL345), l'utilisateur doit pouvoir sélectionner l'accélération affichée en appuyant sur un bouton.
Pour ce faire, le système sera composé des éléments suivants :
* NIOS 2
* RAM
* TIMER
* PIO (7-Segments)
* PIO (Boutons)
* ADXL345

Les éléments seront connectés entre eux de la façon suivante :
![archESN11_Lab3](https://user-images.githubusercontent.com/77203492/213121516-edf589ff-17f9-4000-8246-1784d0a06a0d.png)

On peut voir que le *Timer* aura une interruption qui va timer la prise de mesure toutes les secondes, le *bouton "MODE"* aura aussi un interruptions pour sélectionner quelle valeur afficher. Ensuite on aura un bloc *OPENCORE I2C* qui va permettre la liaison entre le bus Avalon et *l'accéléromètre*. On retrouve finalement les différents *afficheurs* et la *RAM*.

## Platform Designer
L'architecture posée sur le papier passons à sa réalisation sur **Platform Designer** :
![image](https://user-images.githubusercontent.com/77203492/213122491-83e74e88-99f4-4ebd-8c2d-17014af44abf.png)

Sur l'architecture ci-dessus, on relie les différents blocs communiquants par le biais du **Bus Avalon** sur la broche **data_master** du *Nios 2 Processor*, on active la broche **irq** sur le *bouton* et le *time*r, on exporte les conduits **external_connection** du *bouton*, des *7segments*, du *bloc I2C*. Sans oublier évidemment de configurer le Timer, pour qu'il génère une impulsion toutes les secondes.

## Top Level design
L'architecture étant réalisée on peut désormais passer à l'étape de **description Hardware** du système, en VHDL :
Sur celui-ci rien de très particulier, on définit quels sont les *signaux d'entrée et de sorties* du système et on *instancit les différents afficheurs et le système complet*. Petite spécificité à noter, la sortie i2c_cs_n est à "1" et sera assignée au pin *GSENSOR_CS_N* qui correspond au Chip select et permet à l'accéléromètre d'être détecter.

## Pin Assignement
Passons maintenant au pin assignement, on reprend la documentation du DE10-Lite et on ajoute cette fois-ci les pins correspondants :
**PIN_AB15** : GSENSOR_SCLK -> i2c_scl;
**PIN_V11** : GSENSOR_SDI -> i2c_sda;
**PIN_AB16** : GSENSOR_CS_N -> i2c_cs_n;

# Software Description
Dans cette partie je vais m'attarder sur la description du fonctionnement de chacun des programmes :
## read_adxl_simple
Dans ce programme, je propose une simple implémentation de l'adxl en polling et affichage des valeurs d'accélérations dans le terminal, avec la création de 2 fonctions qui sont **read_addr(int addr)** et **write_to_addr(int value, int addr)** qui permettent de communiquer avec l'adxl à travers le bus I2C.

## read_adxl_calibrer
Dans ce programme, je propose une solution au système attendu, tout d'abord je définis les fonctions suivantes :
```C
void write_to_addr(int value,int addr); // Ecrire la valeur "value" à l'addresse "addr" de l'ADXL
int read_addr(int addr); // Permet de lire la valeur situé à l'addresse "addr" de l'ADXL
void ADXL_calibrage(); // Permet de calibrer l'ADXL pour obtenir les mesures les plus précises d'accélérations
void ADXL_config(); // Permet de configurer les registres DATA_FORMAT et POWER_CTL de l'ADXL 
static void handle_button_interrupts(void* context, alt_u32 id); // Routine d'interruption du bouton
static void init_button_pio(); // Initialisation de la routine d'interruption 
static void handle_timer_interrupt(void *context); // Routine d'interruption du Timer
static void init_timer(); // Initialisation de la routine d'interruption
void affiche_7seg(short number); // Fonction d'affichage d'un nombre "number" sur les 7segments
void init_7seg(); // Initialisation des 7 segments
short conversion(int number); // Fonction de conversion qui permet de calculer la "vraie" valeur d'accélération.
```
### Calibrage
Il est nécessaire pour obtenir des mesures cohérentes d'accélérations de calibrer l'ADXL, dans sa documentation, le constructeur nous indique 3 registres à controler pour définir nos offsets de mesures qui sont :
* OFSX (0x1E)
* OFSY (0x1F)
* OFSZ (0x20)

Pour calibrer, j'ai réalisé la démarche suivante :
* Définir les offset à 0
* Mesurer les valeurs d'accélérations
* Convertir cette valeur en "vraie" valeur avec le calcul suivant : 

*Supponsons la valeur suivante sur Z : (0x740)_16 -> (1856)_10 -> On multiplie par 3,9 -> 17260 on multiplie par 0,001 (mg -> g) -> 17,26 g*

* Calculer l'offset nécessaire pour que Z = 9.8g, X = 0g, Y = 0g

*On définit finalement les macros suivantes d'offset :*
```C
#define OFFSET_X 2
#define OFFSET_Y 6
#define OFFSET_Z 20
```

### Routine Interruption Bouton
```C
static void handle_button_interrupts(void* context, alt_u32 id){
	alt_printf("IRQ Button \n");
	flag_data+=1; // flag_data, variable globale de controle d'affichage
	if (flag_data>2)flag_data=0;
  
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BOUTON_BASE, 0x03);
}
```
### Routine Interruption Timer
```C
static void handle_timer_interrupt(void *context){
	int data1,data2=0;
	alt_printf("IRQ Timer : \n");
	data1 = read_addr(ADXL_X1);
	//alt_printf("\tDATAX1 = 0x%x ",data1);
	data2 = read_addr(ADXL_X0);
	//alt_printf("\tDATAX0 = 0x%x",data2);
	dataX = (data2) |(data1<<8); // Calcul de la valeur de X en "collant" DATAX1 et DATAX0
	
	data1 = read_addr(ADXL_Y1);
	//alt_printf("\tDATAY1 = 0x%x ",data1);
	data2 = read_addr(ADXL_Y0);
	//alt_printf("\tDATAY0 = 0x%x",data2);
	dataY = (data2) |(data1<<8); // Calcul de la valeur de Y en "collant" DATAY1 et DATAY0
	
	data1 = read_addr(ADXL_Z1);
	//alt_printf("\tDATAZ1 = 0x%x ",data1);
	data2 = read_addr(ADXL_Z0);
	//alt_printf("\tDATAZ0 = 0x%x",data2);
	dataZ = (data2) |(data1<<8); // Calcul de la valeur de Z en "collant" DATAZ1 et DATAZ0
	
	switch(flag_data){ // Selon le flag_data on affiche la bonne valeur sur les afficheurs
		case 0 :
			dataX = conversion(dataX);
			affiche_7seg(dataX);
			break;
		case 1 :
			dataY = conversion(dataY);
			affiche_7seg(dataY);
			break;
		case 2 :
			dataZ = conversion(dataZ);
			affiche_7seg(dataZ);
			break;
		default :
			affiche_7seg(0);
	}
	alt_printf("X = 0x%x - Y = 0x%x - Z = 0x%x\n",dataX,dataY,dataZ);
	
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_BASE, 0);
}
```

# Résultat 
Ci-dessous une vidéo de fonctionnement du système :

https://user-images.githubusercontent.com/77203492/213132360-ece91e29-d0ba-4c25-a9ba-3e47756f5af2.mp4

# Conclusion 
Avec le lab, j'ai compris comment communiquer avec l'accéléromètre par le biais du bus I2C, comment le configurer. Ainsi que la réalisation de l'architecture du système.
