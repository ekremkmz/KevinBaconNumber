#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct veriTutucu{
	struct veri *data;
	struct veriTutucu *next;
};
typedef struct veriTutucu dataTutucu;
struct veri{
	char name[150];//Veri ismi
	int sayi;//Kevin bacon sayisi
	dataTutucu *veriler;//Film ve oyuncular arasindaki iliskileri tutan veriTutucu
	dataTutucu *sonVeri;//Eklenecek her veri icin veriler in sonunu tutan sonVeri pointeri
	struct veri *next;//Listedeki sonraki veriyi tutan pointer
};
typedef struct veri node;
void listeYazdir(node *liste){
	node *iter;
	iter = liste;
	while(iter->next != NULL){
		printf("%s\n",iter->name);
		iter=iter->next;
	}
}
void yeniVeri(char *parca,node *hedefIter){
	strcpy(hedefIter->name,parca);
	if(hedefIter->name[strlen(hedefIter->name)-1] == '\n'){//Yazilan verinin sonunda end of line karakteri var mý diye kontrol eden blok
	hedefIter->name[strlen(hedefIter->name)-1] = '\0';
		strcpy(parca,hedefIter->name);
	}
	hedefIter->sayi = -1;//Alinan her verinin sayisi -1 (hesaplanmadi) e esitlenir
	hedefIter->next = (node*)malloc(sizeof(node));
	hedefIter->next->veriler = (dataTutucu*)malloc(sizeof(dataTutucu));
	hedefIter->next->sonVeri = hedefIter->next->veriler;//Veriler listesinin en sonunu gosteren pointer in guncellenmesi
	hedefIter->next->veriler->data = NULL;
	hedefIter->next->veriler->next = NULL;
}
void veriEkle(node *hedefIter, node *kaynakIter){
	dataTutucu *dataIter;//Verilerde gezinebilmek icin veriTutucu iteration i
	dataIter = hedefIter->sonVeri;
	dataIter->data = kaynakIter ;//Kaynak verinin hedef verideki son veriye atanmasi
	dataIter->next = (dataTutucu*)malloc(sizeof(dataTutucu));
	hedefIter->sonVeri = dataIter->next;//Son veri nin guncellenmesi
	dataIter->next->data = NULL;
	dataIter->next->next = NULL;
}
void veriAl(FILE *fp,node *filmList,node *oyuncuList){
	char veri[10000]="";//Dosyanin ilk satirini tutabilecek boyutta char array i
	char *filmadi;//Filmin adi tutulan char pointer i
	char *parca;//Oyuncu isimlerini tutan char pointer ý
	node *filmIter;//Film listesinde gezinmek icin iteration
	node *oyuncuIter;//Oyuncu listesinde gezinmek icin iteration
	dataTutucu *dataIter;//Film yada oyuncu verilerinde ggezinmek icin iteration
	int found;//Kontrol degiskeni
	filmIter = filmList;
	while(fgets(veri,10000,fp)){//Veriden yeni satýr al
		filmadi = strtok(veri, "/");
		yeniVeri(filmadi, filmIter);//film eklendi
		parca = strtok(NULL, "/");
		
		while(parca!=NULL ){
			
			found = 0;
			oyuncuIter = oyuncuList;
			while(found == 0 && oyuncuIter->veriler->data != NULL){
				if(strcmp(oyuncuIter->name,parca) == 0){//Alýnan isim oyuncu listesinde var mi diye kontrol eden blok
					found = 1;
				}
				else{
					oyuncuIter = oyuncuIter->next;
				}
			}
			if(found == 0){//Isim oyuncu listesinde yoksa yeni veri olusturan blok
				yeniVeri(parca, oyuncuIter);
			}
			veriEkle(oyuncuIter, filmIter);//Oyuncu verilerine filmi ekleyen fonksiyon
			veriEkle(filmIter, oyuncuIter);//Film verilerine oyuncuyu ekleyen fonksiyon
			parca = strtok(NULL, "/");
		}
		filmIter = filmIter->next;
	}
	filmIter->next = NULL;
	while(oyuncuIter->veriler->data != NULL){//Oyuncu listesinin sonuna giden ve son elemani NULL a esitleyen blok
		oyuncuIter = oyuncuIter->next;
	}
	oyuncuIter = NULL;
}
void listFree(node *liste){
	node *iter;
	dataTutucu *iter2;
	while(liste->next->veriler->data != NULL){
		while(liste->veriler->data != NULL){
			iter2=liste->veriler->next;
			free(liste->veriler);
			liste->veriler=iter2;
		}
		free(liste->veriler);
		iter=liste->next;
		free(liste);
		liste=iter;	
	}
	free(liste);
}
void recHepsi(int *sayilar,node* filmList,int sayi){
	node* filmIter;//Film listesinde gezinmek icin iteration
	filmIter = filmList;
	dataTutucu *dataIter1,*dataIter2;
	int found = 0;//Yeni film bulunduguna dair kontrol degiskeni
	int tmpInt = 0;//Bulunan her oyuncu icin arttýrýlan degisken
	
	while(filmIter->next != NULL){
		if(filmIter->sayi == sayi){
			dataIter1 = filmIter->veriler;
			while(dataIter1->data != NULL){
				if(dataIter1->data->sayi == sayi){//Oncesinde kisi bazli hesaplama yapilmissa etkilenmemesi icin, oyuncunun sayisi ayni degere hesaplanmissa kontrol degiskenini 1 yapan satir
					found = 1;
					tmpInt++;
				}
				if(dataIter1->data->sayi == -1){//Hesaplanmamis oyunculari hesapla
					dataIter1->data->sayi = sayi;
					dataIter2 = dataIter1->data->veriler;//Yeni hesaplanan oyuncunun filmlerini al
					while(dataIter2->data != NULL){//Yeni hesaplanan oyuncunun tum filmlerine 1 fazla ata
						if(dataIter2->data->sayi == -1){
							dataIter2->data->sayi = sayi+1;
							found = 1;//Yeni baglantili  film bulundu						
							}
							dataIter2 = dataIter2->next;
					}
					tmpInt++;
				}
				dataIter1 = dataIter1->next;
			}
		}
		filmIter = filmIter->next;
	}
	sayilar[sayi] = tmpInt;//Bulunan oyuncu sayisayini listeye atayan satir
	if(found == 1){//Yeni sayiya sahip olan film olmussa sonraki Kevin Bacon sayisina sahip filmleri tara
		recHepsi(sayilar,filmList,sayi+1);
	}
	
}
void recKisi(char *kisi,int *sayilar,node* filmList,int sayi){
	node *filmIter;//Film listesinde gezinmek icin iteration
	filmIter = filmList;
	dataTutucu *dataIter1,*dataIter2;
	int found = 0;//Yeni film bulunduguna dair kontrol degiskeni
	int tmpInt = 0;//Bulunan her oyuncu icin arttýrýlan degisken
	
	while(filmIter->veriler->data != NULL){//Filmlistesinin sonuna gelinmediði sürece
		if(filmIter->sayi == sayi){
			dataIter1 = filmIter->veriler;//Filmin oyuncu listesi aliniyor
			while(dataIter1->data != NULL){
				if(dataIter1->data->sayi == -1){//Hesaplanmamis oyunculari hesapla
					dataIter1->data->sayi = sayi;
					if(strcmp(dataIter1->data->name,kisi) == 0){//Oyuncu bulundu ise fonksiyonu sonlandir
						return ;
					}
					dataIter2 = dataIter1->data->veriler;//Yeni hesaplanan oyuncunun filmlerini al
					while(dataIter2->data != NULL){//Yeni hesaplanan oyuncunun tum filmlerine 1 fazla ata
						if(dataIter2->data->sayi == -1){
							dataIter2->data->sayi = sayi+1;
							found = 1;//Yeni baglantili  film bulundu						
						}
						dataIter2 = dataIter2->next;
					}
					tmpInt++;
				}
				dataIter1 = dataIter1->next;//Filmdeki sýradaki oyuncu verisi
			}
		}
		filmIter = filmIter->next;
	}
	sayilar[sayi] = tmpInt;//Fonksiyon return etmedi ise bulunan oyuncu sayisayini listeye atayan satir
	if(found == 1){//Oyuncu bulunamadi ise bir sonraki Kevin Bacon sayisina sahip filmleri tara
		recKisi(kisi,sayilar,filmList,sayi+1);
	}
}
int hepsiniHesapla(int *sayilar,node* filmList,node* oyuncuList){
	int i;//Kevin bacon sayisi sonsuz olanlarin sayisinin tutuldugu degisken
	int a;//Dongu degiskeni
	node *oyuncuIter,*filmIter;//Film ve oyuncu listelerinde gezinmek icin iterationlar
	dataTutucu *dataIter1;//Verilerde gezinmek icin iteration lar
	oyuncuIter=oyuncuList;
	int sayi = 0;
	if(sayilar[0] == 0){//Hepsini hesapla fonksiyonu daha önce çaðýrýlmýþsa bu bloga girilmez
		while(strcmp("Bacon, Kevin",oyuncuIter->name) != 0){
			oyuncuIter = oyuncuIter->next;
		}
		sayilar[sayi]++;
		sayi++;
		dataIter1 = oyuncuIter->veriler;
	
		while(dataIter1->data != NULL){//Kevin bacon'in filmlerine 1 atandi
			dataIter1->data->sayi = sayi;
			dataIter1 = dataIter1->next;
		}
		filmIter = filmList;
		
		recHepsi(sayilar,filmList,sayi);
	}
	i=0;
	oyuncuIter = oyuncuList;
	while(oyuncuIter->veriler->data != NULL){
		if(oyuncuIter->sayi == -1 || oyuncuIter->sayi == -2){
			oyuncuIter->sayi = -2;//-2 sonsuz demek
			i++;
		}
		oyuncuIter = oyuncuIter->next;
	}
	for(a=0;a<15;a++){
		if(sayilar[a] != 0){
			printf("\nKavin Bacon sayisi %d olan aktor sayisi: %d",a,sayilar[a]);
		}
	}
	return i;
}
void kisiHesapla(char *kisi,int *sayilar,node* filmList,node* oyuncuList){
	int tmpSayi;//Bulunduysa oyuncunun kevin bacon sayisini tutan degisken
	node *oyuncuIter;//Oyuncu listesinde gezinmek icin iteration
	node *filmIter;//Film listesinde gezinmek icin iteration
	node *tmp;//Bulunan oyuncu adresini tutan pointer
	dataTutucu *dataIter1,*dataIter2;//Oyuncu ve film verilerinde gezinmek icin veriTutucu iteration i
	oyuncuIter=oyuncuList;
	int sayi = 0;
	oyuncuIter=oyuncuList;
	while(oyuncuIter->veriler->data != NULL && strcmp(kisi,oyuncuIter->name) != 0){//Hesaplanacak kisi bulunuyor
			oyuncuIter=oyuncuIter->next;
	}
	tmp = oyuncuIter;//Oyuncu adresi tmp ye ataniyor
	if(tmp->veriler->data == NULL){//Girdi listede yoksa return eden blok
		printf("\nActor listede bulunamadi. Devam etmek icin bir tusa basin.\n");
		getch();
		return;
	}
	
	oyuncuIter=oyuncuList;
	if(tmp->sayi == -1 && sayilar[0] == 0 && strcmp(kisi,"Bacon, Kevin") != 0){//sayilar[0] degerine sadece hepsiniHesapla() fonksiyonunda 1 atanir ve hepsi hesaplanmissa tekrar aranmasina gerek yoktur
		while(strcmp("Bacon, Kevin",oyuncuIter->name) != 0){
			oyuncuIter=oyuncuIter->next;
		}
		sayi++;
		dataIter1=oyuncuIter->veriler;//Kevin Bacon in filmlerinin oldugu veriler adresi iteration a atandi
	
		while(dataIter1->data != NULL){//Kevin bacon'in filmlerine 1 atandi
			dataIter1->data->sayi = sayi;
			dataIter1 = dataIter1->next;
		}
		filmIter = filmList;
		while(filmIter->next != NULL){//Simdiye kadar hesaplanmis en yüksek Kevin Bacon sayisini listede ariyor
			if(filmIter->sayi > sayi){
				sayi = filmIter->sayi;
			}
			filmIter = filmIter->next;
		}
		recKisi(kisi,sayilar,filmList,sayi);//Bulunan sayiya gore arama basliyor yada en buyuk Kevin Bacon sayisindan devam ediyor
	}
	else if(strcmp(kisi,"Bacon, Kevin") == 0){//Girilen kisi Kevin Bacon in kendisi ise return ediliyor
		printf("%s in Kevin Bacon sayisi 0'dir.",kisi);
		return;
	}
	tmpSayi = tmp->sayi;
	if(tmpSayi == -1 || tmpSayi == -2){//Fonksiyon calismasina ragmen sayisi hala -1(hesaplanmadi) ise sayisi -2(sonsuz) olarak degistiriliyor
		tmp->sayi = -2;
		printf("%s in Kevin Bacon sayisi sonsuzdur.",tmp->name);
		return;
	}
		
	
	printf("%s in Kevin Bacon sayisi %d'dir.",kisi,tmpSayi);
	//Girilen kisiden Kevin Bacon a olan en kisa yolun yazdirilmasi
	printf("\n%d  %s ---->",tmpSayi,tmp->name);
	
	while(tmpSayi != 1){
		dataIter1 = tmp->veriler;
		while(dataIter1->data->sayi != tmpSayi){//Oyuncunun baglantili filmi bulunuyor
			dataIter1 = dataIter1->next;
		}
		dataIter2 = dataIter1->data->veriler;
		tmpSayi--;
		while(dataIter2->data->sayi != tmpSayi){//Filmdeki baglantili oyuncu bulunuyor
			dataIter2 = dataIter2->next;
		}
		printf(" %s\n   %s ----> %s\n%d  %s ---->",dataIter1->data->name,dataIter1->data->name,dataIter2->data->name,tmpSayi,dataIter2->data->name);
		tmp = dataIter2->data;
	}
	dataIter1 = tmp->veriler;
	while(dataIter1->data->sayi != tmpSayi){//Oyuncunun baglantili filmi bulunuyor
		dataIter1 = dataIter1->next;
	}
	printf(" %s\n   %s ---->",dataIter1->data->name,dataIter1->data->name);
	printf(" Bacon, Kevin\n");
}
int main(){
	int i;
	char input[150]="";//Alinacak text dosyasinin adini ve gerekli yerde aranacak kisinin adini tutan char array i
	int sayilar[15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//Hangi Kevin Bacon sayisindan kac tane oldugunu tutan array
	char choose='0';//Calisma tipini tutan char degeri
	FILE *fp;//Dosyayi gosterecek file pointer i
	printf("Dosya adresi girin.(Orn:text.txt)\n");
	scanf("%s",input);
	getchar();
	fp = fopen(input,"r+");//Pointer dosyayi gosteriyor
	if(fp == NULL){
		printf("Hatali dosya ismi!");
		return;
	}
	node *filmList = (node*)malloc(sizeof(node));//Film listesi olusturuluyor
	node *oyuncuList = (node*)malloc(sizeof(node));//Oyuncu listesi olusturuluyor
	
	filmList->next = NULL;
	oyuncuList->next = NULL;
	
	filmList->veriler = (dataTutucu*)malloc(sizeof(dataTutucu));//Film in verileri olusturuluyor
	oyuncuList->veriler = (dataTutucu*)malloc(sizeof(dataTutucu));//Oyuncu nun verileri olusturuluyor
	
	filmList->veriler->data = NULL;
	filmList->veriler->next = NULL;
	filmList->sonVeri = filmList->veriler;
	
	oyuncuList->veriler->data = NULL;
	oyuncuList->veriler->next = NULL;
	oyuncuList->sonVeri = oyuncuList->veriler;//Eklenecek her veri icin veriler in sonunu tutan sonVeri pointeri
	
	printf("\nDosyadan veriler aliniyor. Dosya buyuklugune gore bekleme suresi artabilir.(ornek: input-3.txt 10 dakika)\n");
	veriAl(fp,filmList,oyuncuList);//Dosyadan verileri alan fonksiyon
	
	while(choose!='5'){
		printf("\n-----------------------------------------------------------------------------------------------------------------------");
		printf("\n1)Butun aktorlerin Kevin Bacon sayisini hesapla.(Bir kez yapildiginda hicbir aktor icin tekrar hesaplama gerektirmez)\n");
		printf("\n2)Belirli bir aktorun Kevin Bacon sayisini hesapla.(Daha once hesaplanan bir aktor icin tekrar hesaplama gerekmez)\n");
		printf("\n3)Oyuncu listesini yazdir.\n");
		printf("\n4)Film listesini yazdir.\n");
		printf("\n5)Cikis\n");
		
		choose=getchar();
		getchar();
		switch(choose){
			case '1':
				printf("\nTum aktorler hesaplaniyor...\n");
				i=hepsiniHesapla(sayilar,filmList,oyuncuList);
				printf("\nKavin Bacon sayisi sonsuz olan aktor sayisi: %d",i);
				break;
			case '2':
				printf("\nAktor ismi giriniz.(ornek:Bacon, Kevin)\n");
				scanf("%[^\n]s",input);
				printf("\nAktor hesaplaniyor...\n");
				kisiHesapla(input,sayilar,filmList,oyuncuList);
				getchar();
				break;
			case '3':
				listeYazdir(oyuncuList);
				break;
			case '4':
				listeYazdir(filmList);
				break;
			case '5':
				break;
			default:
				printf("\nHatali girdi!\n");
				break;
		}
	}
	listFree(filmList);
	listFree(oyuncuList);
	fclose(fp);
	return 0;
}
