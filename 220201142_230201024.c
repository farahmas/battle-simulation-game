#define NOGDI
#define NOUSER
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <curl/curl.h>

#define ANAHTAR 100
#define MDEGER 500
#define MCANAVAR 50
#define MKAHRAMAN 50
#define MARASTIRMA 20
#define MBIRIMTURU 30
#define TARAF_MKAHRAMAN 10
#define TARAF_MCANAVAR 10
#define MKARAKTER 100
#define HUCRE_MBIRIM 100

#define IZGARA 10
#define HUCRE 60
#define ALAN_G (IZGARA * HUCRE)
#define ALAN_Y (IZGARA * HUCRE)

typedef struct
{
    char type[ANAHTAR];
    int count;
    Texture2D texture;
    char name[ANAHTAR];
} HucreYapisi;

typedef struct
{
    char name[50];
    int count;
    float health;
    float max_health;
    Texture2D texture;
} Karakter;

HucreYapisi grid[IZGARA][IZGARA] = {0};

typedef struct
{
    const char *name;
    int kontrol;
} ArastirmaYapisi;

ArastirmaYapisi arastirmalar[] = {
    {"savunma_ustaligi", 0},
    {"saldiri_gelistirmesi", 0},
    {"elit_egitim", 0},
    {"kusatma_ustaligi", 0},
    {NULL, 0}
};

typedef struct
{
    char name[ANAHTAR];
    char research_name[ANAHTAR];
    int research_level;
    char hero[TARAF_MKAHRAMAN][MDEGER];
    char monster[TARAF_MCANAVAR][MDEGER];
    int hero_count;
    int monster_count;
    int varg_binicileri;
    int mizrakcilar;
    int ork_dovusculeri;
    int troller;
    int suvariler;
    int okcular;
    int piyadeler;
    int kusatma_makineleri;
    Karakter characters[MKARAKTER];
    int character_count;

} Taraf;

typedef struct
{
    char name[ANAHTAR];
    int effect_value;
    char effect_type[ANAHTAR];
    char description[MDEGER];
} CanavarYapisi;

typedef struct
{
    char name[ANAHTAR];
    char bonus_type[ANAHTAR];
    int bonus_value;
    char description[MDEGER];
} KahramanYapisi;

typedef struct
{
    char name[ANAHTAR];
    int level_1_value;
    int level_2_value;
    int level_3_value;
    char level_1_description[MDEGER];
    char level_2_description[MDEGER];
    char level_3_description[MDEGER];
} ArastirmaOzellik;

typedef struct
{
    char name[ANAHTAR];
    int attack;
    int defense;
    int health;
    int critical_chance;
} BirimTuru;

typedef struct
{
    Taraf ork_legi;
    Taraf insan_imparatorlugu;
    CanavarYapisi creatures[MCANAVAR];
    int creature_count;
    KahramanYapisi heroes[MKAHRAMAN];
    int hero_count;
    ArastirmaOzellik researches[MARASTIRMA];
    int research_count;
    BirimTuru unit_types[MBIRIMTURU];
    int unit_type_count;
    int ork_unit_count;
    int human_unit_count;
    Karakter characters[MKARAKTER];
    int character_count;

} OyunYapisi;

OyunYapisi ilk_struct = {0};

//hesaplama için buradan itibaren

typedef struct {
    char name[ANAHTAR];
    float attack;
    float defense;
    float health;
    float critical_chance;
    int count;
    float current_health;
} SavasBirimi;

typedef struct {
    SavasBirimi units[MBIRIMTURU];
    int unit_count;
    char hero[TARAF_MKAHRAMAN][MDEGER];
    char monster[TARAF_MCANAVAR][MDEGER];
    int hero_count;
    int monster_count;
    char research_name[ANAHTAR];
    int research_level;
    float total_attack;
    float total_defense;
    int total_units; 
} SavasTarafYapisi;


Texture2D arkaplanResmi;
Texture2D boslukResmi;
Texture2D kahramanResmi; 
Texture2D canavarResmi;
Texture2D insan1png;
Texture2D insan2png;
Texture2D insan3png;
Texture2D insan4png;
Texture2D ork1png;
Texture2D ork2png;
Texture2D ork3png;
Texture2D ork4png;


FILE *savas_sim;

void savas_durumunu_yazdir(SavasTarafYapisi *taraf, const char *taraf_adi, int turs_sayisi);
void toplam_saldiri_ve_savunma_yazdir(SavasTarafYapisi *taraf, const char *taraf_adi);
void arastirma_uygula(SavasTarafYapisi *taraf, OyunYapisi *yapi);
void insan_canavar_ve_kahraman_etkileri(SavasTarafYapisi *taraf, OyunYapisi *yapi);
void ork_kahraman_ve_canavar_etkileri(SavasTarafYapisi *taraf, OyunYapisi *yapi);
void savas_ana_fonksiyon(OyunYapisi *yapi);
Texture2D ResimYuklemeKontrolu(const char *dosyaY);
void resim_yukle();
void izgara_baslat();
void birimleri_yerlestir(const char *biri_turu, int sayi, Texture2D resim, int taraf);
void kahraman_ve_canavar_yerlestir(const char *birim_tipi, Taraf *taraf, Texture2D resim, int tarafS);


//linkten senaryo dosyasını indirme fonksiyonu
void dosya_indirme() {   
    const char *linkler[] = { 
        "https://yapbenzet.org.tr/1.json",

        "https://yapbenzet.org.tr/2.json",

        "https://yapbenzet.org.tr/3.json",

        "https://yapbenzet.org.tr/4.json",

        "https://yapbenzet.org.tr/5.json",

        "https://yapbenzet.org.tr/6.json",

        "https://yapbenzet.org.tr/7.json",

        "https://yapbenzet.org.tr/8.json",

        "https://yapbenzet.org.tr/9.json",

        "https://yapbenzet.org.tr/10.json"
    };
    int linkSayisi = sizeof(linkler) / sizeof(linkler[0]);
    int secim;

    printf("Lutfen indirmek istediginiz dosyanin numarasini secin (1-%d): ", linkSayisi);
    scanf("%d", &secim);
    
    while (secim < 1 || secim > linkSayisi) {
        printf("Gecersiz sayi, lutfen 1 ile %d arasinda bir sayi girin:\n", linkSayisi);
        scanf("%d", &secim);
    } 

    const char *secilenLink = linkler[secim - 1]; 
    
    CURL *curl_ptr;
    FILE *file_ptr;
    CURLcode res;

    char dosyaadi[FILENAME_MAX] = "senaryo.json";
    
    file_ptr = fopen(dosyaadi, "wb");                   
    if (!file_ptr) {
        perror("Dosya acilamadi.");
        return;
    }

    curl_ptr = curl_easy_init();
    if (curl_ptr) {
        curl_easy_setopt(curl_ptr, CURLOPT_URL, secilenLink);
        curl_easy_setopt(curl_ptr, CURLOPT_WRITEDATA, file_ptr);
        res = curl_easy_perform(curl_ptr);
        
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() basarisiz: %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl_ptr);
    }

    fclose(file_ptr);
    printf("Dosya indirildi: %s\n", dosyaadi);
}



void kes_bic(char *str)
{
    if (str == NULL)
        return;

    char *bas = str;
    while (*bas && isspace((unsigned char)*bas))
    {
        bas++;
    }

    if (bas != str)
    {
        memmove(str, bas, strlen(bas) + 1);
    }

    char *son = str + strlen(str) - 1;
    while (son > str && isspace((unsigned char)*son))
    {
        *son = '\0';
        son--;
    }
}

void tirnak_temizle(char *str)
{
    char *a = str;
    char *b = str;
    int tirnak_arasi = 0;

    while (*a)
    {
        if (*a == '"')
        {
            tirnak_arasi = !tirnak_arasi;
            a++;
        }
        else if (*a == ',' && !tirnak_arasi)
        {
            *b++ = *a++;
            while (*a == ' ')
                a++;
        }
        else
        {
            *b++ = *a++;
        }
    }
    *b = '\0';
}

char *dosya_oku(const char *dosya)
{
    FILE *file_ptr = fopen(dosya, "rb");
    if (file_ptr == NULL)
    {
        printf("Dosya acilamadi: %s\n", dosya);
        return NULL;
    }

    fseek(file_ptr, 0, SEEK_END);
    long dosya_boyutu = ftell(file_ptr);
    fseek(file_ptr, 0, SEEK_SET);

    char *icerik = (char *)malloc(dosya_boyutu + 1);
    if (icerik == NULL)
    {
        printf("Dosya icin hafiza ayirma hatasi %s\n", dosya);
        fclose(file_ptr);
        return NULL;
    }

    size_t read_size = fread(icerik, 1, dosya_boyutu, file_ptr);
    icerik[read_size] = '\0';

    fclose(file_ptr);

    char *a = icerik;
    char *b = icerik;
    while (*a)
    {
        if (*a != '\r')
        {
            *b = *a;
            b++;
        }
        a++;
    }
    *b = '\0';

    return icerik;
}
void parantez_temizle(char *str)
{
    size_t uzunluk = strlen(str);
    while (uzunluk > 0 && (str[uzunluk - 1] == ']' || str[uzunluk - 1] == '}' || isspace(str[uzunluk - 1])))
    {
        str[uzunluk - 1] = '\0';
        uzunluk--;
    }
}
void adi_temizle(char *ad)
{
    tirnak_temizle(ad);
    kes_bic(ad);
    parantez_temizle(ad);
}

void json_taraf_ayristirma(const char *bilgi)
{

    printf("\nsenaryo.json ayristiriliyor...\n");
    fprintf(savas_sim, "\nsenaryo.json ayristiriliyor...\n");

    char *taraf_basi;

    // Ork Legini ayrıştırma
    taraf_basi = strstr(bilgi, "\"ork_legi\"");
    if (taraf_basi)
    {
        strcpy(ilk_struct.ork_legi.name, "ork_legi");

        // Ork Legi Araştırma Ayrıştırma
        char *arastirma_basi = strstr(taraf_basi, "\"arastirma_seviyesi\":");
        if (arastirma_basi)
        {
            char *arastirma_icerik_basi = strchr(arastirma_basi, '{');
            char *arastirma_icerik_sonu = strchr(arastirma_icerik_basi, '}');
            if (arastirma_icerik_basi && arastirma_icerik_sonu)
            {
                char arastirma_obj[256];
                strncpy(arastirma_obj, arastirma_icerik_basi + 1, arastirma_icerik_sonu - arastirma_icerik_basi - 1);
                arastirma_obj[arastirma_icerik_sonu - arastirma_icerik_basi - 1] = '\0';

                if (strstr(arastirma_obj, "\"savunma_ustaligi\":"))
                {
                    strcpy(ilk_struct.ork_legi.research_name, "savunma_ustaligi");
                    sscanf(strstr(arastirma_obj, "\"savunma_ustaligi\":"), "\"savunma_ustaligi\": %d", &ilk_struct.ork_legi.research_level);
                }
                else if (strstr(arastirma_obj, "\"saldiri_gelistirmesi\":"))
                {
                    strcpy(ilk_struct.ork_legi.research_name, "saldiri_gelistirmesi");
                    sscanf(strstr(arastirma_obj, "\"saldiri_gelistirmesi\":"), "\"saldiri_gelistirmesi\": %d", &ilk_struct.ork_legi.research_level);
                }
                else if (strstr(arastirma_obj, "\"elit_egitim\":"))
                {
                    strcpy(ilk_struct.ork_legi.research_name, "elit_egitim");
                    sscanf(strstr(arastirma_obj, "\"elit_egitim\":"), "\"elit_egitim\": %d", &ilk_struct.ork_legi.research_level);
                }
                else if (strstr(arastirma_obj, "\"kusatma_ustaligi\":"))
                {
                    strcpy(ilk_struct.ork_legi.research_name, "kusatma_ustaligi");
                    sscanf(strstr(arastirma_obj, "\"kusatma_ustaligi\":"), "\"kusatma_ustaligi\": %d", &ilk_struct.ork_legi.research_level);
                }
            }
        }

        // Ork Legi Kahraman Ayrıştırma
        char *kahraman_basi = strstr(taraf_basi, "\"kahramanlar\":");
        if (kahraman_basi)
        {
            char *kahraman_icerik_basi = strchr(kahraman_basi, '[') + 1;
            char *kahraman_icerik_sonu = strchr(kahraman_icerik_basi, ']');
            if (kahraman_icerik_sonu && kahraman_icerik_sonu > kahraman_icerik_basi)
            {
                char *simdiki_kahraman = kahraman_icerik_basi;
                int kahraman_indeksi = 0;
                while (simdiki_kahraman < kahraman_icerik_sonu && kahraman_indeksi < TARAF_MKAHRAMAN)
                {
                    char *sonraki_virgul = strchr(simdiki_kahraman, ',');
                    char *kahraman_adi_sonu = sonraki_virgul ? sonraki_virgul : kahraman_icerik_sonu;
                    size_t kahraman_adi_uzunluk = kahraman_adi_sonu - simdiki_kahraman;

                    strncpy(ilk_struct.ork_legi.hero[kahraman_indeksi], simdiki_kahraman, kahraman_adi_uzunluk);
                    ilk_struct.ork_legi.hero[kahraman_indeksi][kahraman_adi_uzunluk] = '\0';
                    adi_temizle(ilk_struct.ork_legi.hero[kahraman_indeksi]);

                    kahraman_indeksi++;
                    simdiki_kahraman = sonraki_virgul ? sonraki_virgul + 1 : kahraman_icerik_sonu;
                }
                ilk_struct.ork_legi.hero_count = kahraman_indeksi;
            }
            else
            {
                ilk_struct.ork_legi.hero_count = 0;
            }
        }
        else
        {
            ilk_struct.ork_legi.hero_count = 0;
        }

        // Ork Legi Canavarlar Ayrıştırma
        char *canavar_basi = strstr(taraf_basi, "\"canavarlar\":");
        if (canavar_basi)
        {
            char *canavar_icerik_basi = strchr(canavar_basi, '[') + 1;
            char *canavar_icerik_sonu = strchr(canavar_icerik_basi, ']');
            if (canavar_icerik_sonu && canavar_icerik_sonu > canavar_icerik_basi)
            {
                char *simdiki_canavar = canavar_icerik_basi;
                int canavar_indeksi = 0;
                while (simdiki_canavar < canavar_icerik_sonu && canavar_indeksi < TARAF_MCANAVAR)
                {
                    char *sonraki_virgul = strchr(simdiki_canavar, ',');
                    char *canavar_adi_sonu = sonraki_virgul ? sonraki_virgul : canavar_icerik_sonu;
                    size_t canavar_ad_uzunluk = canavar_adi_sonu - simdiki_canavar;

                    strncpy(ilk_struct.ork_legi.monster[canavar_indeksi], simdiki_canavar, canavar_ad_uzunluk);
                    ilk_struct.ork_legi.monster[canavar_indeksi][canavar_ad_uzunluk] = '\0';
                    adi_temizle(ilk_struct.ork_legi.monster[canavar_indeksi]);

                    canavar_indeksi++;
                    simdiki_canavar = sonraki_virgul ? sonraki_virgul + 1 : canavar_icerik_sonu;
                }
                ilk_struct.ork_legi.monster_count = canavar_indeksi;
            }
            else
            {
                ilk_struct.ork_legi.monster_count = 0;
            }
        }
        else
        {
            ilk_struct.ork_legi.monster_count = 0;
        }
        // Ork Legi birimlerini ayrıştırma
        char *birimler_basi = strstr(taraf_basi, "\"birimler\":");
        if (birimler_basi)
        {
            char *birim_str;
            birim_str = strstr(birimler_basi, "\"varg_binicileri\":");
            if (birim_str)
                sscanf(birim_str, "\"varg_binicileri\": %d", &ilk_struct.ork_legi.varg_binicileri);
            birim_str = strstr(birimler_basi, "\"mizrakcilar\":");
            if (birim_str)
                sscanf(birim_str, "\"mizrakcilar\": %d", &ilk_struct.ork_legi.mizrakcilar);
            birim_str = strstr(birimler_basi, "\"ork_dovusculeri\":");
            if (birim_str)
                sscanf(birim_str, "\"ork_dovusculeri\": %d", &ilk_struct.ork_legi.ork_dovusculeri);
            birim_str = strstr(birimler_basi, "\"troller\":");
            if (birim_str)
                sscanf(birim_str, "\"troller\": %d", &ilk_struct.ork_legi.troller);
        }
    }

    //  İnsan İmparatorluğu Ayrıştırma
    taraf_basi = strstr(bilgi, "\"insan_imparatorlugu\"");
    if (taraf_basi)
    {
        strcpy(ilk_struct.insan_imparatorlugu.name, "insan_imparatorlugu");

        // İnsan Araştırma Ayrıştırma
        char *arastirma_basi = strstr(taraf_basi, "\"arastirma_seviyesi\":");
        if (arastirma_basi)
        {
            char *arastirma_icerik_basi = strchr(arastirma_basi, '{');
            char *arastirma_icerik_sonu = strchr(arastirma_icerik_basi, '}');
            if (arastirma_icerik_basi && arastirma_icerik_sonu)
            {
                char arastirma_obj[256];
                strncpy(arastirma_obj, arastirma_icerik_basi + 1, arastirma_icerik_sonu - arastirma_icerik_basi - 1);
                arastirma_obj[arastirma_icerik_sonu - arastirma_icerik_basi - 1] = '\0';

                if (strstr(arastirma_obj, "\"savunma_ustaligi\":"))
                {
                    strcpy(ilk_struct.insan_imparatorlugu.research_name, "savunma_ustaligi");
                    sscanf(strstr(arastirma_obj, "\"savunma_ustaligi\":"), "\"savunma_ustaligi\": %d", &ilk_struct.insan_imparatorlugu.research_level);
                }
                else if (strstr(arastirma_obj, "\"saldiri_gelistirmesi\":"))
                {
                    strcpy(ilk_struct.insan_imparatorlugu.research_name, "saldiri_gelistirmesi");
                    sscanf(strstr(arastirma_obj, "\"saldiri_gelistirmesi\":"), "\"saldiri_gelistirmesi\": %d", &ilk_struct.insan_imparatorlugu.research_level);
                }
                else if (strstr(arastirma_obj, "\"elit_egitim\":"))
                {
                    strcpy(ilk_struct.insan_imparatorlugu.research_name, "elit_egitim");
                    sscanf(strstr(arastirma_obj, "\"elit_egitim\":"), "\"elit_egitim\": %d", &ilk_struct.insan_imparatorlugu.research_level);
                }
                else if (strstr(arastirma_obj, "\"kusatma_ustaligi\":"))
                {
                    strcpy(ilk_struct.insan_imparatorlugu.research_name, "kusatma_ustaligi");
                    sscanf(strstr(arastirma_obj, "\"kusatma_ustaligi\":"), "\"kusatma_ustaligi\": %d", &ilk_struct.insan_imparatorlugu.research_level);
                }
            }
        }

        // İnsan Kahraman Ayrıştırma
        char *kahraman_basi = strstr(taraf_basi, "\"kahramanlar\":");
        if (kahraman_basi)
        {
            char *kahraman_icerik_basi = strchr(kahraman_basi, '[') + 1;
            char *kahraman_icerik_sonu = strchr(kahraman_icerik_basi, ']');
            if (kahraman_icerik_sonu && kahraman_icerik_sonu > kahraman_icerik_basi)
            {
                char *simdiki_kahraman = kahraman_icerik_basi;
                int kahraman_indeksi = 0;
                while (simdiki_kahraman < kahraman_icerik_sonu && kahraman_indeksi < TARAF_MKAHRAMAN)
                {
                    char *sonraki_virgul = strchr(simdiki_kahraman, ',');
                    char *kahraman_adi_sonu = sonraki_virgul ? sonraki_virgul : kahraman_icerik_sonu;
                    size_t kahraman_adi_uzunluk = kahraman_adi_sonu - simdiki_kahraman;

                    strncpy(ilk_struct.insan_imparatorlugu.hero[kahraman_indeksi], simdiki_kahraman, kahraman_adi_uzunluk);
                    ilk_struct.insan_imparatorlugu.hero[kahraman_indeksi][kahraman_adi_uzunluk] = '\0';
                    adi_temizle(ilk_struct.insan_imparatorlugu.hero[kahraman_indeksi]);

                    kahraman_indeksi++;
                    simdiki_kahraman = sonraki_virgul ? sonraki_virgul + 1 : kahraman_icerik_sonu;
                }
                ilk_struct.insan_imparatorlugu.hero_count = kahraman_indeksi;
            }
            else
            {
                ilk_struct.insan_imparatorlugu.hero_count = 0;
            }
        }
        else
        {
            ilk_struct.insan_imparatorlugu.hero_count = 0;
        }

        // İnsan Canavar Ayrıştırma
        char *canavar_basi = strstr(taraf_basi, "\"canavarlar\":");
        if (canavar_basi)
        {
            char *canavar_icerik_basi = strchr(canavar_basi, '[') + 1;
            char *canavar_icerik_sonu = strchr(canavar_icerik_basi, ']');
            if (canavar_icerik_sonu && canavar_icerik_sonu > canavar_icerik_basi)
            {
                char *simdiki_canavar = canavar_icerik_basi;
                int canavar_indeksi = 0;
                while (simdiki_canavar < canavar_icerik_sonu && canavar_indeksi < TARAF_MCANAVAR)
                {
                    char *sonraki_virgul = strchr(simdiki_canavar, ',');
                    char *canavar_adi_sonu = sonraki_virgul ? sonraki_virgul : canavar_icerik_sonu;
                    size_t canavar_ad_uzunluk = canavar_adi_sonu - simdiki_canavar;

                    strncpy(ilk_struct.insan_imparatorlugu.monster[canavar_indeksi], simdiki_canavar, canavar_ad_uzunluk);
                    ilk_struct.insan_imparatorlugu.monster[canavar_indeksi][canavar_ad_uzunluk] = '\0';
                    adi_temizle(ilk_struct.insan_imparatorlugu.monster[canavar_indeksi]);

                    canavar_indeksi++;
                    simdiki_canavar = sonraki_virgul ? sonraki_virgul + 1 : canavar_icerik_sonu;
                }
                ilk_struct.insan_imparatorlugu.monster_count = canavar_indeksi;
            }
            else
            {
                ilk_struct.insan_imparatorlugu.monster_count = 0;
            }
        }
        else
        {
            ilk_struct.insan_imparatorlugu.monster_count = 0;
        }

        // İnsan birimlerini ayrıştırma
        char *birimler_basi = strstr(taraf_basi, "\"birimler\":");
        if (birimler_basi)
        {
            char *birim_str;
            birim_str = strstr(birimler_basi, "\"suvariler\":");
            if (birim_str)
                sscanf(birim_str, "\"suvariler\": %d", &ilk_struct.insan_imparatorlugu.suvariler);
            birim_str = strstr(birimler_basi, "\"okcular\":");
            if (birim_str)
                sscanf(birim_str, "\"okcular\": %d", &ilk_struct.insan_imparatorlugu.okcular);
            birim_str = strstr(birimler_basi, "\"piyadeler\":");
            if (birim_str)
                sscanf(birim_str, "\"piyadeler\": %d", &ilk_struct.insan_imparatorlugu.piyadeler);
            birim_str = strstr(birimler_basi, "\"kusatma_makineleri\":");
            if (birim_str)
                sscanf(birim_str, "\"kusatma_makineleri\": %d", &ilk_struct.insan_imparatorlugu.kusatma_makineleri);
        }
    }
}

void creatures_json_ayristirma(const char *bilgi)
{
    printf("\ncreatures.json ayristiriliyor...\n");
    fprintf(savas_sim, "\ncreatures.json ayristiriliyor...\n");
    const char *ptr = bilgi;
    int canavar_sayisi = 0;
    int derinlik = 0;
    char irk[ANAHTAR] = {0};
    char yaratik[ANAHTAR] = {0};

    while (*ptr)
    {
        if (*ptr == '{')
        {
            derinlik++;
        }
        else if (*ptr == '}')
        {
            derinlik--;
            if (derinlik == 1)
            {
                memset(irk, 0, sizeof(irk));
            }
            else if (derinlik == 2)
            {
                memset(yaratik, 0, sizeof(yaratik));
            }
        }
        else if (*ptr == '"')
        {
            const char *son = strchr(ptr + 1, '"');
            if (son)
            {
                size_t uzunluk = son - ptr - 1;
                if (derinlik == 1)
                {
                    strncpy(irk, ptr + 1, uzunluk);
                    irk[uzunluk] = '\0';
                }
                else if (derinlik == 2)
                {
                    strncpy(yaratik, ptr + 1, uzunluk);
                    yaratik[uzunluk] = '\0';
                }
                else if (derinlik == 3 && strncmp(ptr + 1, "etki_degeri", 11) == 0)
                {
                    CanavarYapisi c = {0};
                    snprintf(c.name, sizeof(c.name), "%s_%s", irk, yaratik);

                    const char *value_start = strchr(son + 1, '"');
                    if (value_start)
                    {
                        sscanf(value_start + 1, "%d", &c.effect_value);
                    }

                    const char *type_start = strstr(son, "\"etki_turu\"");
                    if (type_start)
                    {
                        type_start = strchr(type_start + 11, '"');
                        if (type_start)
                        {
                            sscanf(type_start + 1, "%[^\"]", c.effect_type);
                        }
                    }

                    const char *desc_start = strstr(son, "\"aciklama\"");
                    if (desc_start)
                    {
                        desc_start = strchr(desc_start + 10, '"');
                        if (desc_start)
                        {
                            sscanf(desc_start + 1, "%[^\"]", c.description);
                        }
                    }

                    if (ilk_struct.creature_count < MCANAVAR)
                    {
                        ilk_struct.creatures[ilk_struct.creature_count] = c;
                        ilk_struct.creature_count++;
                    }
                    else
                    {
                        printf("Uyari: Maksimum canavar sayisina ulasildi. Fazla canavarlar yoksayildi..\n");
                    }

                    canavar_sayisi++;
                }
                ptr = son;
            }
        }
        ptr++;
    }
}

void heroes_json_ayristirma(const char *bilgi)
{
    printf("\nheroes.json ayristiriliyor...\n");
    fprintf(savas_sim, "\nheroes.json ayristiriliyor...\n");
    const char *ptr = bilgi;
    int kahraman_sayisi = 0;
    int derinlik = 0;
    char irk[ANAHTAR] = {0};
    char kahraman[ANAHTAR] = {0};

    while (*ptr)
    {
        if (*ptr == '{')
        {
            derinlik++;
        }
        else if (*ptr == '}')
        {
            derinlik--;
            if (derinlik == 1)
            {
                memset(irk, 0, sizeof(irk));
            }
            else if (derinlik == 2)
            {
                memset(kahraman, 0, sizeof(kahraman));
            }
        }
        else if (*ptr == '"')
        {
            const char *son = strchr(ptr + 1, '"');
            if (son)
            {
                size_t uzunluk = son - ptr - 1;
                if (derinlik == 1)
                {
                    strncpy(irk, ptr + 1, uzunluk);
                    irk[uzunluk] = '\0';
                }
                else if (derinlik == 2)
                {
                    strncpy(kahraman, ptr + 1, uzunluk);
                    kahraman[uzunluk] = '\0';
                }
                else if (derinlik == 3 && strncmp(ptr + 1, "bonus_turu", 10) == 0)
                {
                    KahramanYapisi h = {0};
                    snprintf(h.name, sizeof(h.name), "%s_%s", irk, kahraman);

                    const char *type_start = strchr(son + 1, '"');
                    if (type_start)
                    {
                        sscanf(type_start + 1, "%[^\"]", h.bonus_type);
                    }

                    const char *value_start = strstr(son, "\"bonus_degeri\"");
                    if (value_start)
                    {
                        value_start = strchr(value_start + 14, '"');
                        if (value_start)
                        {
                            sscanf(value_start + 1, "%d", &h.bonus_value);
                        }
                    }

                    const char *desc_start = strstr(son, "\"aciklama\"");
                    if (desc_start)
                    {
                        desc_start = strchr(desc_start + 10, '"');
                        if (desc_start)
                        {
                            sscanf(desc_start + 1, "%[^\"]", h.description);
                        }
                    }

                    if (ilk_struct.hero_count < MKAHRAMAN)
                    {
                        ilk_struct.heroes[ilk_struct.hero_count] = h;
                        ilk_struct.hero_count++;
                    }
                    else
                    {
                        printf("Uyari: Maksimum kahraman sayisina ulasildi. Fazla kahramanlar yok sayildi.\n");
                    }

                    kahraman_sayisi++;
                }
                ptr = son;
            }
        }
        ptr++;
    }
}

void research_json_ayristir(const char *bilgi)
{
    printf("\nresearch.json ayristiriliyor...\n");
    fprintf(savas_sim, "\nresearch.json ayristiriliyor...\n");
    const char *ptr = bilgi;
    int arastirma_sayisi = 0;

    // Başlangıç parantezi geç
    ptr = strchr(ptr, '{');
    if (!ptr)
        return;
    ptr++;

    while (*ptr)
    {
        // Sonraki research'i bul
        ptr = strstr(ptr, "\"");
        if (!ptr)
            break;

        // Research adını çıkar
        const char *ad_sonu = strchr(ptr + 1, '"');
        if (!ad_sonu)
            break;
        size_t ad_uzunlugu = ad_sonu - (ptr + 1);

        // Aranan research olup olmadığını kontrol et
        ArastirmaYapisi *current_research = NULL;
        for (int i = 0; arastirmalar[i].name != NULL; i++)
        {
            if (strncmp(ptr + 1, arastirmalar[i].name, ad_uzunlugu) == 0 &&
                strlen(arastirmalar[i].name) == ad_uzunlugu)
            {
                current_research = &arastirmalar[i];
                break;
            }
        }

        // Eğer değilse sonrakine git
        if (!current_research)
        {
            ptr = ad_sonu + 1;
            continue;
        }

        ArastirmaOzellik research = {0};
        strncpy(research.name, current_research->name, sizeof(research.name) - 1);

        // Resarch'in başlangıcına git
        ptr = strchr(ad_sonu, '{');
        if (!ptr)
            break;

        //Seviye bilgisini çıkar
        const char *level1 = strstr(ptr, "\"seviye_1\":");
        const char *level2 = strstr(ptr, "\"seviye_2\":");
        const char *level3 = strstr(ptr, "\"seviye_3\":");

        if (level1 && level2 && level3)
        {
            sscanf(strstr(level1, "\"deger\":"), "\"deger\": \"%d\"", &research.level_1_value);
            sscanf(strstr(level2, "\"deger\":"), "\"deger\": \"%d\"", &research.level_2_value);
            sscanf(strstr(level3, "\"deger\":"), "\"deger\": \"%d\"", &research.level_3_value);

            sscanf(strstr(level1, "\"aciklama\":"), "\"aciklama\": \"%[^\"]\"", research.level_1_description);
            sscanf(strstr(level2, "\"aciklama\":"), "\"aciklama\": \"%[^\"]\"", research.level_2_description);
            sscanf(strstr(level3, "\"aciklama\":"), "\"aciklama\": \"%[^\"]\"", research.level_3_description);

            kes_bic(research.level_1_description);
            kes_bic(research.level_2_description);
            kes_bic(research.level_3_description);

            if (ilk_struct.research_count < MARASTIRMA)
            {
                ilk_struct.researches[ilk_struct.research_count] = research;
                ilk_struct.research_count++;
            }
            else
            {
                printf("Uyari: Maksimum arastirma sayisina ulasildi. Fazla arastirmalar yoksayildi.\n");
            }

            current_research->kontrol = 1;
            arastirma_sayisi++;
        }

        // Sonuna git
        ptr = strchr(ptr, '}');
        if (!ptr)
            break;
        ptr++;
    }

    //Tüm araştırmaların ayrıştırılıp ayrıştırılmadığını kontrol et
    for (int i = 0; arastirmalar[i].name != NULL; i++)
    {
        if (!arastirmalar[i].kontrol)
        {
            printf("Uyari: Aratirma '%s' bulunamadi.\n", arastirmalar[i].name);
        }
    }
}

void unit_types_json_ayristir(const char *bilgi)
{
    printf("\nunit_types.json ayristiriliyor...\n");
    fprintf(savas_sim, "\nunit_types.json ayristiriliyor...\n");
    const char *ptr = bilgi;
    int derinlik = 0;
    char taraf[ANAHTAR] = {0};
    char birim[ANAHTAR] = {0};

    ilk_struct.ork_unit_count = 0;
    ilk_struct.human_unit_count = 0;
    ilk_struct.unit_type_count = 0;

    while (*ptr)
    {
        if (*ptr == '{')
        {
            derinlik++;
        }
        else if (*ptr == '}')
        {
            derinlik--;
            if (derinlik == 1)
            {
                memset(taraf, 0, sizeof(taraf));
            }
            else if (derinlik == 2)
            {
                memset(birim, 0, sizeof(birim));
            }
        }
        else if (*ptr == '"')
        {
            const char *son = strchr(ptr + 1, '"');
            if (son)
            {
                size_t len = son - ptr - 1;
                if (derinlik == 1)
                {
                    strncpy(taraf, ptr + 1, len);
                    taraf[len] = '\0';
                }
                else if (derinlik == 2)
                {
                    strncpy(birim, ptr + 1, len);
                    birim[len] = '\0';
                }
                else if (derinlik == 3 && strncmp(ptr + 1, "saldiri", 7) == 0)
                {
                    BirimTuru u = {0};
                    strncpy(u.name, birim, sizeof(u.name) - 1);
                    u.name[sizeof(u.name) - 1] = '\0';

                    const char *saldiri_basi = strchr(son + 1, ':');
                    if (saldiri_basi)
                        sscanf(saldiri_basi + 1, "%d", &u.attack);

                    const char *savunma_basi = strstr(son, "\"savunma\"");
                    if (savunma_basi)
                    {
                        savunma_basi = strchr(savunma_basi + 9, ':');
                        if (savunma_basi)
                            sscanf(savunma_basi + 1, "%d", &u.defense);
                    }

                    const char *saglik_basi = strstr(son, "\"saglik\"");
                    if (saglik_basi)
                    {
                        saglik_basi = strchr(saglik_basi + 8, ':');
                        if (saglik_basi)
                            sscanf(saglik_basi + 1, "%d", &u.health);
                    }

                    const char *kritik_basi = strstr(son, "\"kritik_sans\"");
                    if (kritik_basi)
                    {
                        kritik_basi = strchr(kritik_basi + 13, ':');
                        if (kritik_basi)
                            sscanf(kritik_basi + 1, "%d", &u.critical_chance);
                    }

                    if (ilk_struct.unit_type_count < MBIRIMTURU)
                    {
                        ilk_struct.unit_types[ilk_struct.unit_type_count] = u;
                        ilk_struct.unit_type_count++;

                        if (strcmp(taraf, "ork_legi") == 0)
                        {
                            ilk_struct.ork_unit_count++;
                        }
                        else if (strcmp(taraf, "insan_imparatorlugu") == 0)
                        {
                            ilk_struct.human_unit_count++;
                        }
                    }
                    else
                    {
                        printf("Uyari: Maksimum unit_type sayisina ulasildi. Fazla unit_types yok sayildi.\n");
                    }
                }
                ptr = son;
            }
        }
        ptr++;
    }
}

//İlk değerleri kopyalamak, araştırma, kahraman ve canavar etkilerini en başta uygulamak için fonksiyon
void savasi_baslat(SavasTarafYapisi *saldiran, SavasTarafYapisi *savunan, OyunYapisi *yapi) { 
    saldiran->unit_count = 0;
    savunan->unit_count = 0;
    saldiran->total_units = 0;  
    savunan->total_units = 0;

    printf("\n\nBirim sayisi: %d\n\n", yapi->human_unit_count);
    printf("Debug: Birim eklenmeden önce toplam: %d\n", saldiran->total_units);


    // İnsan İmparatorluğu birimlerini ekle
    for (int i = 0; i < yapi->human_unit_count; i++) {
        BirimTuru *bt = &yapi->unit_types[i];
        
            SavasBirimi *sb = &saldiran->units[saldiran->unit_count++];
            strcpy(sb->name, bt->name);
            sb->attack = bt->attack;
            sb->defense = bt->defense;
            sb->health = bt->health;
            sb->critical_chance = bt->critical_chance;
            sb->current_health = bt->health;

            if (strstr(bt->name, "piyadeler") != NULL) {
                sb->count = yapi->insan_imparatorlugu.piyadeler;
            } else if (strstr(bt->name, "okcular") != NULL) {
                sb->count = yapi->insan_imparatorlugu.okcular;
            } else if (strstr(bt->name, "suvariler") != NULL) {
                sb->count = yapi->insan_imparatorlugu.suvariler;
            } else if (strstr(bt->name, "kusatma_makineleri") != NULL) {
                sb->count = yapi->insan_imparatorlugu.kusatma_makineleri;
            }
            saldiran->total_units += sb->count;

            printf("Debug: Birim %s eklendi, sayisi: %d, toplam: %d\n", sb->name, sb->count, saldiran->total_units);
    }

    // Ork Legi birimlerini ekle
    for (int i = 0; i < yapi->ork_unit_count; i++) {
        BirimTuru *bt = &yapi->unit_types[yapi->human_unit_count + i];
            SavasBirimi *sb = &savunan->units[savunan->unit_count++];
            strcpy(sb->name, bt->name);
            sb->attack = bt->attack;
            sb->defense = bt->defense;
            sb->health = bt->health;
            sb->critical_chance = bt->critical_chance;
            sb->current_health = bt->health;

            if (strstr(bt->name, "ork_dovusculeri") != NULL) {
                sb->count = yapi->ork_legi.ork_dovusculeri;
            } else if (strstr(bt->name, "mizrakcilar") != NULL) {
                sb->count = yapi->ork_legi.mizrakcilar;
            } else if (strstr(bt->name, "varg_binicileri") != NULL) {
                sb->count = yapi->ork_legi.varg_binicileri;
            } else if (strstr(bt->name, "troller") != NULL) {
                sb->count = yapi->ork_legi.troller;
            }
            savunan->total_units += sb->count;
    }

    // Kahraman, canavar ve araştırma bilgilerini kopyala
    saldiran->hero_count = yapi->insan_imparatorlugu.hero_count; //////
    for (int i = 0; i < saldiran->hero_count; i++) {
        strcpy(saldiran->hero[i], yapi->insan_imparatorlugu.hero[i]);
    }
    saldiran->monster_count = yapi->insan_imparatorlugu.monster_count;
    for (int i = 0; i < saldiran->monster_count; i++) {
        strcpy(saldiran->monster[i], yapi->insan_imparatorlugu.monster[i]);
    }
    strcpy(saldiran->research_name, yapi->insan_imparatorlugu.research_name);
    saldiran->research_level = yapi->insan_imparatorlugu.research_level;

    savunan->hero_count = yapi->ork_legi.hero_count;
    for (int i = 0; i < savunan->hero_count; i++) {
        strcpy(savunan->hero[i], yapi->ork_legi.hero[i]);
    }
    savunan->monster_count = yapi->ork_legi.monster_count;
    for (int i = 0; i < savunan->monster_count; i++) {
        strcpy(savunan->monster[i], yapi->ork_legi.monster[i]);
    }

    strcpy(savunan->research_name, yapi->ork_legi.research_name);
    savunan->research_level = yapi->ork_legi.research_level;
    

    printf("\n--- Arastirma, Kahraman ve Canavar Etkileri Oncesi (Tur 0, Baslangic)---\n");
    fprintf(savas_sim, "\n--- Arastirma, Kahraman ve Canavar Etkileri Oncesi (Tur 0, Baslangic)---\n");
    savas_durumunu_yazdir(saldiran, "Insan Imparatorlugu", 0);
    savas_durumunu_yazdir(savunan, "Ork Legi", 0);

    printf("\n--- Senaryodan Gelen Canavar, Kahraman ve Arastirma Bilgileri ---\n\n");
    fprintf(savas_sim, "\n--- Senaryodan Gelen Canavar, Kahraman ve Arastirma Bilgileri ---\n\n");

    printf("Insan Imparatorlugu:\n");
    fprintf(savas_sim, "Insan Imparatorlugu:\n");

    printf("  Kahramanlar:\n");
    fprintf(savas_sim, "  Kahramanlar:\n");
    for (int i = 0; i < saldiran->hero_count; i++) {
        printf("    - %s\n", saldiran->hero[i]);
        fprintf(savas_sim, "    - %s\n", saldiran->hero[i] );
    }
    printf("  Canavarlar:\n");
    for (int i = 0; i < saldiran->monster_count; i++) {
        printf("    - %s\n", saldiran->monster[i]);
        fprintf(savas_sim, "    - %s\n", saldiran->monster[i] );
    }
    printf("  Arastirma: %s (Seviye %d)\n", saldiran->research_name, saldiran->research_level);
    fprintf(savas_sim, "  Arastirma: %s (Seviye %d)\n", saldiran->research_name, saldiran->research_level);

    printf("\nOrk Legi:\n");
    fprintf(savas_sim, "\nOrk Legi:\n");
    printf("  Kahramanlar:\n");
    fprintf(savas_sim, "  Kahramanlar:\n");

    for (int i = 0; i < savunan->hero_count; i++) {
        printf("    - %s\n", savunan->hero[i]);
        fprintf(savas_sim, "    - %s\n", savunan->hero[i]);
    }
    printf("  Canavarlar:\n");
    fprintf(savas_sim, "  Canavarlar:\n");
    for (int i = 0; i < savunan->monster_count; i++) {
        printf("    - %s\n", savunan->monster[i]);
        fprintf(savas_sim, "    - %s\n", savunan->monster[i]);
    }
    printf("  Arastirma: %s (Seviye %d)\n", savunan->research_name, savunan->research_level);
    fprintf(savas_sim, "  Arastirma: %s (Seviye %d)\n", savunan->research_name, savunan->research_level);

    printf("\nArastirma, Kahraman ve Canavar Etkileri Uygulaniyor...\n\n");
    fprintf(savas_sim, "\nArastirma, Kahraman ve Canavar Etkileri Uygulaniyor...\n\n");

    //Araştırma etkileri
    arastirma_uygula(saldiran, yapi);
    arastirma_uygula(savunan, yapi);

    //Kahraman ve Canavar Etkileri
    insan_canavar_ve_kahraman_etkileri(saldiran, yapi);
    ork_kahraman_ve_canavar_etkileri(savunan, yapi);
}

void insan_canavar_ve_kahraman_etkileri(SavasTarafYapisi *insanlar, OyunYapisi *yapi) {
    printf("\nInsan Imparatorlugu Kahraman ve Canavar Etkileri Uygulaniyor...\n\n");
    fprintf(savas_sim, "\nInsan Imparatorlugu Kahraman ve Canavar Etkileri Uygulaniyor...\n\n");

    // Kahramanların etkilerini uygula
    for (int i = 0; i < insanlar->hero_count; i++) {
        for (int j = 0; j < yapi->hero_count; j++) {
            if (strstr(yapi->heroes[j].name, insanlar->hero[i]) != NULL) {
                printf("Kahraman %s'in etkisi kontrol ediliyor:\n", yapi->heroes[j].name);
                fprintf(savas_sim, "Kahraman %s'in etkisi kontrol ediliyor:\n", yapi->heroes[j].name);

                for (int k = 0; k < insanlar->unit_count; k++) {
                    SavasBirimi *birim = &insanlar->units[k];
                    if (birim->count > 0 && strstr(yapi->heroes[j].description, insanlar->units[k].name) != NULL) {
                        float bonus = 1 + (yapi->heroes[j].bonus_value / 100.0f);

                        
                        if (strstr(yapi->heroes[j].description, "saldiri") != NULL) {
                            birim->attack *= bonus;
                            printf("  %s biriminin saldiri gucu %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name,(float)yapi->heroes[j].bonus_value, birim->attack);
                            fprintf(savas_sim, "  %s biriminin saldiri gucu %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name, (float)yapi->heroes[j].bonus_value, birim->attack);
                        } else if (strstr(yapi->heroes[j].description, "savunma") != NULL) {
                            birim->defense *= bonus;
                            printf("  %s biriminin savunma gucu %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name, (float)yapi->heroes[j].bonus_value, birim->defense);
                            fprintf(savas_sim, "  %s biriminin savunma gucu %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name, (float)yapi->heroes[j].bonus_value, birim->defense);
                        } else if (strstr(yapi->heroes[j].description, "kritik") != NULL) {
                            birim->critical_chance *= bonus;
                            printf("  %s biriminin kritik sans %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name, (float)yapi->heroes[j].bonus_value, birim->critical_chance);
                            fprintf(savas_sim, "  %s biriminin kritik sans %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name, (float)yapi->heroes[j].bonus_value, birim->critical_chance);
                        }
                    }
                }
            }
        }
    }

    // Canavarların etkilerini uygula
    for (int i = 0; i < insanlar->monster_count; i++) {
        for (int j = 0; j < yapi->creature_count; j++) {
            if (strstr(yapi->creatures[j].name, insanlar->monster[i]) != NULL) {
                printf("\nCanavar %s'in etkisi kontrol ediliyor:\n", yapi->creatures[j].name);
                fprintf(savas_sim, "\nCanavar %s'in etkisi kontrol ediliyor:\n", yapi->creatures[j].name);

                for (int k = 0; k < insanlar->unit_count; k++) {
                    SavasBirimi *birim = &insanlar->units[k];
                    if (birim->count > 0 && strstr(yapi->creatures[j].description, birim->name) != NULL) {
                        float bonus = 1 + (yapi->creatures[j].effect_value / 100.0f);
                        
                        if (strcmp(yapi->creatures[j].effect_type, "saldiri") == 0) {
                            birim->attack *= bonus;
                            printf("  %s biriminin saldiri gucu %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name, (float)yapi->creatures[j].effect_value, birim->attack);
                            fprintf(savas_sim, "  %s biriminin saldiri gucu %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name, (float)yapi->creatures[j].effect_value, birim->attack);
                        } else if (strcmp(yapi->creatures[j].effect_type, "savunma") == 0) {
                            birim->defense *= bonus;
                            printf("  %s biriminin savunma gucu %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name, (float)yapi->creatures[j].effect_value, birim->defense);
                            fprintf(savas_sim, "  %s biriminin savunma gucu %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name, (float)yapi->creatures[j].effect_value, birim->defense);
                        } else if (strcmp(yapi->creatures[j].effect_type, "kritik_sans") == 0) {
                            birim->critical_chance *= bonus;
                            printf("  %s biriminin kritik sans %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name, (float)yapi->creatures[j].effect_value, birim->critical_chance);
                            fprintf(savas_sim, "  %s biriminin kritik sans %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name, (float)yapi->creatures[j].effect_value, birim->critical_chance);
                        }
                    }
                }
            }
        }
    }
}

void ork_kahraman_ve_canavar_etkileri(SavasTarafYapisi *orklar, OyunYapisi *yapi) {
    printf("\nOrk Legi Kahraman ve Canavar Etkileri Uygulaniyor...\n\n");
    fprintf(savas_sim, "\nOrk Legi Kahraman ve Canavar Etkileri Uygulaniyor...\n\n");

    // Kahramanların etkilerini uygula
    for (int i = 0; i < orklar->hero_count; i++) {
        for (int j = 0; j < yapi->hero_count; j++) {
            if (strstr(yapi->heroes[j].name, orklar->hero[i]) != NULL) {
                printf("Kahraman %s'in etkisi uygulaniyor:\n", yapi->heroes[j].name);
                fprintf(savas_sim, "Kahraman %s'in etkisi uygulaniyor:\n", yapi->heroes[j].name);

                for (int k = 0; k < orklar->unit_count; k++) {
                    SavasBirimi *birim = &orklar->units[k];
                    if (birim->count > 0 && strstr(yapi->heroes[j].description, orklar->units[k].name) != NULL) {
                        float bonus = 1 + (yapi->heroes[j].bonus_value / 100.0f);
                        
                        if (strcmp(yapi->heroes[j].bonus_type, "saldiri") == 0) {
                            birim->attack *= bonus;
                            printf("  %s biriminin saldiri gucu %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name, (float)yapi->heroes[j].bonus_value, birim->attack);
                            fprintf(savas_sim, "  %s biriminin saldiri gucu %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name, (float)yapi->heroes[j].bonus_value, birim->attack);
                        } else if (strcmp(yapi->heroes[j].bonus_type, "savunma") == 0) {
                            birim->defense *= bonus;
                            printf("  %s biriminin savunma gucu %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name, (float)yapi->heroes[j].bonus_value, birim->defense);
                            fprintf(savas_sim, "  %s biriminin savunma gucu %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name, (float)yapi->heroes[j].bonus_value, birim->defense);
                        } else if (strcmp(yapi->heroes[j].bonus_type, "kritik_sans") == 0) {
                            birim->defense *= bonus;
                            printf("  %s biriminin kritik sansi %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name, (float)yapi->heroes[j].bonus_value, birim->critical_chance);
                            fprintf(savas_sim, "  %s biriminin savunma gucu %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name, (float)yapi->heroes[j].bonus_value, birim->critical_chance);
                        }
                    }  else if(strstr(yapi->heroes[j].description, "tum_birim") != NULL) {
                         float bonus = 1 + (yapi->heroes[j].bonus_value / 100.0f);
                         if (strcmp(yapi->heroes[j].bonus_type, "saldiri") == 0) {
                            birim->attack *= bonus;
                            printf("  %s biriminin saldiri gucu %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name, (float)yapi->heroes[j].bonus_value, birim->attack);
                            fprintf(savas_sim, "  %s biriminin saldiri gucu %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name, (float)yapi->heroes[j].bonus_value, birim->attack);
                        } else if (strcmp(yapi->heroes[j].bonus_type, "savunma") == 0) {
                            birim->defense *= bonus;
                            printf("  %s biriminin savunma gucu %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name, (float)yapi->heroes[j].bonus_value, birim->defense);
                            fprintf(savas_sim, "  %s biriminin savunma gucu %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name, (float)yapi->heroes[j].bonus_value, birim->defense);
                        } else if (strcmp(yapi->heroes[j].bonus_type, "kritik_sans") == 0) {
                            birim->defense *= bonus;
                            printf("  %s biriminin kritik sansi %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name, (float)yapi->heroes[j].bonus_value, birim->critical_chance);
                            fprintf(savas_sim, "  %s biriminin savunma gucu %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name, (float)yapi->heroes[j].bonus_value, birim->critical_chance);
                        }
                    }
                }
            }
        }
    }
    // Canavarların etkilerini uygula
    for (int i = 0; i < orklar->monster_count; i++) {
        for (int j = 0; j < yapi->creature_count; j++) {
            if (strstr(yapi->creatures[j].name, orklar->monster[i]) != NULL) {
                printf("\nCanavar %s'in etkisi uygulaniyor:\n", yapi->creatures[j].name);
                fprintf(savas_sim, "\nCanavar %s'in etkisi uygulaniyor:\n", yapi->creatures[j].name);

                for (int k = 0; k < orklar->unit_count; k++) {
                    SavasBirimi *birim = &orklar->units[k];
                    if (birim->count > 0 && strstr(yapi->creatures[j].description, birim->name) != NULL) {
                        float bonus = 1 + (yapi->creatures[j].effect_value / 100.0f);
                        
                        if (strcmp(yapi->creatures[j].effect_type, "saldiri") == 0) {
                            birim->attack *= bonus;
                            printf("  %s biriminin saldiri gucu %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name, (float)yapi->creatures[j].effect_value, birim->attack);
                            fprintf(savas_sim, "  %s biriminin saldiri gucu %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name, (float)yapi->creatures[j].effect_value, birim->attack);
                        } else if (strcmp(yapi->creatures[j].effect_type, "savunma") == 0) {
                            birim->defense *= bonus;
                            printf("  %s biriminin savunma gucu %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name, (float)yapi->creatures[j].effect_value, birim->defense);
                            fprintf(savas_sim, "  %s biriminin savunma gucu %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name, (float)yapi->creatures[j].effect_value, birim->defense);
                        } else if (strcmp(yapi->creatures[j].effect_type, "kritik_sans") == 0) {
                            birim->critical_chance *= bonus;
                            printf("  %s biriminin kritik sansi %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name, (float)yapi->creatures[j].effect_value, birim->critical_chance);
                            fprintf(savas_sim, "  %s biriminin savunma gucu %%%.2f artirildi. Yeni deger: %.2f\n", 
                                   birim->name, (float)yapi->creatures[j].effect_value, birim->critical_chance);
                    }
                }
            }
        }
    }
}
}
void arastirma_uygula(SavasTarafYapisi *taraf, OyunYapisi *yapi) {
    for (int i = 0; i < yapi->research_count; i++) {
        if (strcmp(yapi->researches[i].name, taraf->research_name) == 0) {
            int bonus_deger;
            if (taraf->research_level == 1) 
               bonus_deger = yapi->researches[i].level_1_value;
            else if (taraf->research_level == 2) 
               bonus_deger = yapi->researches[i].level_2_value;
            else 
               bonus_deger = yapi->researches[i].level_3_value;

            printf("%s arastirmasi uygulaniyor (Seviye %d, Bonus: %d%%)\n", 
                    taraf->research_name, taraf->research_level, bonus_deger);
            fprintf(savas_sim, "%s arastirmasi uygulaniyor (Seviye %d, Bonus: %d%%)\n", 
                   taraf->research_name, taraf->research_level, bonus_deger);

            // Araştırma etkisini birimlere uygula
            for (int j = 0; j < taraf->unit_count; j++) {
                SavasBirimi *birim = &taraf->units[j];
                if (birim->count > 0) {  // Sadece mevcut birimlere uygula
                    if (strcmp(taraf->research_name, "savunma_ustaligi") == 0) {
                        birim->defense *= (1 + bonus_deger / 100.0f);
                        printf("  %s biriminin savunma gucu %%%d artirdi. Yeni deger: %.2f\n", 
                               birim->name, bonus_deger, birim->defense);
                    } else if (strcmp(taraf->research_name, "saldiri_gelistirmesi") == 0) {
                        birim->attack *= (1 + bonus_deger / 100.0f);
                        printf("  %s biriminin saldiri gucu %%%d artirdi. Yeni deger: %.2f\n", 
                               birim->name, bonus_deger, birim->attack);
                    } else if (strcmp(taraf->research_name, "elit_egitim") == 0) {
                        birim->critical_chance *= (1 + bonus_deger / 100.0f);
                        printf("  %s biriminin kritik vurus sansi %%%d artirdi. Yeni deger: %.2f\n", 
                               birim->name, bonus_deger, birim->critical_chance);
                    } else if (strcmp(taraf->research_name, "kusatma_ustaligi") == 0) {
                        if (strstr(birim->name, "kusatma_makineleri") != NULL) {
                            birim->attack *= (1 + bonus_deger / 100.0f);
                            printf("  %s biriminin saldiri gucu %%%d artirdi. Yeni deger: %.2f\n", 
                                   birim->name, bonus_deger, birim->attack);
                        }
                    }
                }
            }
            break; 
        }
    }
}

float  kritik_uygula(SavasTarafYapisi *saldiran, int turSayisi, int saldiriSayisi) {
    float kritik_carpani = 1.0;
    
    for (int i = 0; i < saldiran->unit_count; i++) {
        SavasBirimi *birim = &saldiran->units[i];
        int kritik_orani;
        if(birim->critical_chance >0 )
        {
            kritik_orani = 100 / birim ->critical_chance;
            if(saldiriSayisi % kritik_orani == 0)
            {

               kritik_carpani = 1.5;
               
               birim->attack = (birim->attack * 1.5);
               printf("Tur %d: %s birimi %d. saldirida kritik vurus yapti! Saldiri gucu %.2f'den %.2f'ye yukseldi.\n", 
                   turSayisi, birim->name, saldiriSayisi, birim->attack, birim->attack * kritik_carpani);
               fprintf(savas_sim, "Tur %d: %s birimi %d. saldirida kritik vurus yapti! Saldiri gucu %.2f'den %.2f'ye yukseldi.\n", 
                   turSayisi, birim->name, saldiriSayisi, birim->attack, birim->attack * kritik_carpani);
            }
            
        }        
   }
   return kritik_carpani;
}
void yorgunluk_uygula(SavasTarafYapisi *taraf, int turSayisi) {

    if (turSayisi % 5 != 0 ) return;

    const float yorgunluk_orani = 0.10;

    for (int i = 0; i < taraf->unit_count; i++) {
        SavasBirimi *birim = &taraf->units[i];
        if (birim->count <= 0) continue;

        float yeni_saldiri  = birim->attack * (1 - yorgunluk_orani);
        birim->attack = (yeni_saldiri > 0) ? yeni_saldiri : 1; // Minimum 1 olarak sınırla

        float yeni_savunma = birim->defense * (1 - yorgunluk_orani);
        birim->defense = (yeni_savunma > 0) ? yeni_savunma : 1;

        printf("%s birimi icin yeni degerler:\n", birim->name);
        printf("  Yeni Saldiri Gucu: %.2f\n", birim->attack);
        printf("  Yeni Savunma Gucu: %.2f\n\n", birim->defense);

        fprintf(savas_sim, "%s birimi icin yeni degerler:\n", birim->name);
        fprintf(savas_sim, "  Yeni Saldiri Gucu: %.2f\n", birim->attack);
        fprintf(savas_sim, "  Yeni Savunma Gucu: %.2f\n\n", birim->defense);

        
    }
}
void toplam_saldiri_ve_savunma_hesaplama(SavasTarafYapisi *taraf) { 
    taraf->total_attack = 0;
    taraf->total_defense = 0;
    for (int i = 0; i < taraf->unit_count; i++) {
        taraf->total_attack += taraf->units[i].attack * taraf->units[i].count;
        taraf->total_defense += taraf->units[i].defense * taraf->units[i].count;
    }
}

float net_hasar_hesaplama(SavasTarafYapisi *saldiran, SavasTarafYapisi *savunan) { 
    float hasar;

    if (saldiran->total_attack == 0 || savunan->total_defense >= saldiran->total_attack) {
        hasar =  0;
    }

    hasar = saldiran->total_attack * (1 - (float)savunan->total_defense / saldiran->total_attack);

    return hasar > 0 ? hasar : 0;
}

void hasari_dagit(SavasTarafYapisi *savunan, float net_hasar) { 
    if (net_hasar < 0) net_hasar = 0;

    float toplam_savunma = savunan->total_defense;

    for (int i = 0; i < savunan->unit_count; i++) {
        SavasBirimi *birim = &savunan->units[i];
        if (birim->count <= 0) continue;

        // Birimin savunma oranını hesapla
        float savunma_orani = (birim->defense * birim->count) / toplam_savunma;
        
        // Birime düşen hasarı hesapla
        float birim_hasari = net_hasar * savunma_orani;

        float yeni_saglik = birim->current_health - (birim_hasari / birim->count);

        if (yeni_saglik > 0) 
        {
           birim->current_health = yeni_saglik;
        } 
        else 
        {
           birim->current_health = 0;
        }
        printf("%s birimi %.2f hasar aldi:\n", birim->name, birim_hasari); 
        printf("%s biriminin yeni saglik degeri: %.2f (Birim sayisi: %d)\n", 
               birim->name, birim->current_health, birim->count);

        fprintf(savas_sim, "%s birimi %.2f hasar aldi:\n", birim->name, birim_hasari); 
        fprintf(savas_sim, "%s biriminin yeni saglik degeri: %.2f (Birim sayisi: %d)\n", 
               birim->name, birim->current_health, birim->count);
    }
} 

float toplam_saglik_hesaplama(SavasTarafYapisi *taraf) {
    float toplam_saglik = 0;
    for (int i = 0; i < taraf->unit_count; i++) {
        if (taraf->units[i].count > 0) {
            toplam_saglik += taraf->units[i].current_health;
        }
    }
    return toplam_saglik;
}

void savas_tur_uygulamalari(SavasTarafYapisi *saldiran, SavasTarafYapisi *savunan, OyunYapisi *yapi, int tur_sayisi, int saldiriSayisi) {
    printf("\n=== TUR %d ===\n", tur_sayisi);

    savas_durumunu_yazdir(saldiran, "Saldiran", tur_sayisi);
    savas_durumunu_yazdir(savunan, "Savunan", tur_sayisi);

    //yorgunluk
    if(tur_sayisi % 5 == 0)
    {
     printf("\n\nTur %d: Yorgunluk uygulaniyor. Tum birimlerin saldiri ve savunma gucleri %%10 azaliyor...\n\n", tur_sayisi);
     fprintf(savas_sim, "\n\nTur %d: Yorgunluk uygulaniyor. Tum birimlerin saldiri ve savunma gucleri %%10 azaliyor...\n\n", tur_sayisi);
     yorgunluk_uygula(saldiran, tur_sayisi);
     yorgunluk_uygula(savunan, tur_sayisi);
    }

    // kritik
    float kritik_carpani = kritik_uygula(saldiran, tur_sayisi, saldiriSayisi);

    // Toplam saldırı ve savunma gücünü hesapla
    toplam_saldiri_ve_savunma_hesaplama(saldiran);
    toplam_saldiri_ve_savunma_hesaplama(savunan);

    toplam_saldiri_ve_savunma_yazdir(saldiran, "Sadiran toplam degerler");
    toplam_saldiri_ve_savunma_yazdir(savunan, "Savunan toplma degerler");
    
     // Net hasar
    float net_hasar = net_hasar_hesaplama(saldiran, savunan);
    if (net_hasar >= 0) {
        net_hasar *= kritik_carpani;
        printf("\nToplam Hasar: %.2f\n", net_hasar);
        fprintf(savas_sim, "\nToplam Hasar: %.2f\n", net_hasar);
        hasari_dagit(savunan, net_hasar);
    };

    printf("\nTur Sonu Durumu:");
    fprintf(savas_sim, "\nTur Sonu Durumu:");
    savas_durumunu_yazdir(saldiran, "Saldiran", tur_sayisi);
    savas_durumunu_yazdir(savunan, "Savunan", tur_sayisi);
}

BirimTuru *ada_gore_tur_bul(const char *ad, OyunYapisi *yapi)
{
    printf("Birim turu araniyor: %s\n", ad);
    for (int i = 0; i < yapi->unit_type_count; i++)
    {
        printf("Birim turu kontrol ediliyor: %s\n", yapi->unit_types[i].name);
        if (strcmp(yapi->unit_types[i].name, ad) == 0)
        {
            printf("Birim turu bulundu: %s Saglik: %d\n", yapi->unit_types[i].name, yapi->unit_types[i].health);
            return &yapi->unit_types[i];
        }
    }
    printf("Biri turu %s bulunamadi.\n", ad);
    return NULL;
}

void savas_ana_fonksiyon(OyunYapisi *yapi) {
    SavasTarafYapisi insanlar, orklar;
    
    //Önce insanlar saldıracak, orklar savunacak.
    savasi_baslat(&insanlar, &orklar, yapi);

    printf("\n--- Arastirma, Kahraman ve Canavar Etkileri Sonrasi Durumlar---\n\n");
    fprintf(savas_sim, "\n--- Arastirma, Kahraman ve Canavar Etkileri Sonrasi Durumlar---\n\n");
    savas_durumunu_yazdir(&insanlar, "Insan Imparatorlugu", 0);
    savas_durumunu_yazdir(&orklar, "Ork Legi", 0);


    int tur_sayisi = 1;
    int saldiriSayisiOrk = 0;
    int saldiriSayisiInsan = 0;

    float insan_saglik = toplam_saglik_hesaplama(&insanlar);
    float ork_saglik = toplam_saglik_hesaplama(&orklar);

    
    //Bir faksiyonun toplam sağlığı 0 olana kadar oyun devam eder 
    while (toplam_saglik_hesaplama(&insanlar) > 0 && toplam_saglik_hesaplama(&orklar) > 0) {
        if (tur_sayisi % 2 == 1) {
            saldiriSayisiInsan++;
            savas_tur_uygulamalari(&insanlar, &orklar, yapi, tur_sayisi, saldiriSayisiInsan);
        } else {
            saldiriSayisiOrk++;
            savas_tur_uygulamalari(&orklar, &insanlar, yapi, tur_sayisi, saldiriSayisiOrk);
        }
        tur_sayisi++;
        
        printf("Devam etmek icin bir tusa basin...\n");
        getchar();
    }
    
    printf("Savas %d turda bitti.\n", tur_sayisi - 1);
    fprintf(savas_sim, "Savas %d turda bitti.\n", tur_sayisi - 1);
    
    insan_saglik = toplam_saglik_hesaplama(&insanlar);
    ork_saglik = toplam_saglik_hesaplama(&orklar);
    
    SavasTarafYapisi *kazanan;
    const char *kazanan_isim;
    
    if (insan_saglik > ork_saglik) {
        kazanan = &insanlar;
        kazanan_isim = "Insan Imparatorlugu";
    } else if (ork_saglik > insan_saglik) {
        kazanan = &orklar;
        kazanan_isim = "Ork Legi";
    } else {
        printf("Savas berabere bitti!\n");
        fprintf(savas_sim, "Savas berabere bitti!\n");
        printf("Her iki tarafin kalan sagligi: %.2f, %.2f\n", insan_saglik, ork_saglik);
        fprintf(savas_sim, "Her iki tarafin kalan sagligi: %.2f, %.2f\n", insan_saglik, ork_saglik);
        return;
    }
    
    printf("%s kazandi!\n", kazanan_isim);
    fprintf(savas_sim, "%s kazandi!\n", kazanan_isim);
    printf("Kalan toplam saglik: %.2f\n", toplam_saglik_hesaplama(kazanan));
    fprintf(savas_sim,"Kalan toplam saglik: %.2f\n", toplam_saglik_hesaplama(kazanan));
    printf("\nKazanan tarafin kalan birimleri:\n");
    fprintf(savas_sim, "\nKazanan tarafin kalan birimleri:\n");
  
    for (int i = 0; i < kazanan->unit_count; i++) {
        SavasBirimi *unit = &kazanan->units[i];
        if (unit->count > 0) {
            printf("%s: %d adet, her birinin saglik durumu %.2f\n", 
                   unit->name, unit->count, unit->current_health);
            fprintf(savas_sim, "%s: %d adet, her birinin saglik durumu %.2f\n", 
                   unit->name, unit->count, unit->current_health);

        }
    }

    //2. resim için
    yapi->insan_imparatorlugu.character_count = 0; 
    for (int i = 0; i < kazanan->unit_count; i++)
    {
        SavasBirimi *birim = &kazanan->units[i];
        if (birim->count > 0)
        {
            BirimTuru *birim_turu = ada_gore_tur_bul(birim->name, yapi);

            if (birim_turu != NULL)
            {

                Karakter mevcutKarakter;
                strcpy(mevcutKarakter.name, birim->name); 
                mevcutKarakter.count = birim->count;
                mevcutKarakter.health = birim->current_health;  
                mevcutKarakter.max_health = birim_turu->health; 

                if (yapi->insan_imparatorlugu.character_count < MKARAKTER)
                {
                    yapi->insan_imparatorlugu.characters[yapi->insan_imparatorlugu.character_count++] = mevcutKarakter; 
                }
                else
                {
                    printf("Maksimum sayiya ulasildi. Sonrakiler yok sayilir.\n");
                }
            }
            else
            {
                printf(" %s birim turu bulunamadi!\n", birim->name);
            }
        }
    }

}
void savas_durumunu_yazdir(SavasTarafYapisi *taraf, const char *taraf_adi, int tur_sayisi) {
    printf("\n");
    printf("%s Degerleri: (Tur %d):\n", taraf_adi, tur_sayisi);
    for (int i = 0; i < taraf->unit_count; i++) {
        SavasBirimi *birim = &taraf->units[i];
        if (birim->count > 0) {
            printf("\n");
            printf("  %s:\n", birim->name);
            printf("    Saldiri: %.2f\n", birim->attack);
            printf("    Savunma: %.2f\n", birim->defense);
            printf("    Kritik Sans: %.2f\n", birim->critical_chance);
            printf("    Kalan Birim: %d\n", birim->count);
            printf("    Mevcut Saglik: %.2f\n", birim->current_health);

            fprintf(savas_sim, "\n");
            fprintf(savas_sim, "  %s:\n", birim->name);
            fprintf(savas_sim, "    Saldiri: %.2f\n", birim->attack);
            fprintf(savas_sim, "    Savunma: %.2f\n", birim->defense);
            fprintf(savas_sim, "    Kritik Sans: %.2f\n", birim->critical_chance);
            fprintf(savas_sim, "    Kalan Birim: %d\n", birim->count);
            fprintf(savas_sim, "    Mevcut Saglik: %.2f\n", birim->current_health);


        }
    }

}
void toplam_saldiri_ve_savunma_yazdir(SavasTarafYapisi *taraf, const char *taraf_adi)
{
   
            printf("\n");
            printf("  %s:\n", taraf_adi);
            printf("  Toplam Birim Saiyisi: %d\n", taraf->total_units);
            printf("  Toplam Saldiri Gucu: %.2f\n", taraf->total_attack);
            printf("  Toplam Savunma Gucu: %.2f\n\n", taraf->total_defense);

            fprintf(savas_sim, "\n");
            fprintf(savas_sim, "  %s:\n", taraf_adi);
            fprintf(savas_sim, "  Toplam Birim Sayisi: %d\n", taraf->total_units);
            fprintf(savas_sim, "  Toplam Saldiri Gucu: %.2f\n", taraf->total_attack);
            fprintf(savas_sim, "  Toplam Savunma Gucu: %.2f\n\n", taraf->total_defense);
}
Texture2D ResimYuklemeKontrolu(const char *dosya)
{
    Texture2D resim = LoadTexture(dosya);
    if (resim.id == 0)
    {
        printf("Resim %s yuklenirken hata!\n", dosya);
    }
    else
    {
        printf(" [%s] Basariyla yuklendi.\n", dosya);
    }
    return resim;
}

void resim_yukle()
{
    arkaplanResmi = ResimYuklemeKontrolu("background.png");
    boslukResmi = ResimYuklemeKontrolu("empty.png");
    kahramanResmi = ResimYuklemeKontrolu("super.png");
    canavarResmi = ResimYuklemeKontrolu("monster.png");
    insan1png = ResimYuklemeKontrolu("insan1.png");
    insan2png = ResimYuklemeKontrolu("insan2.png");
    insan3png = ResimYuklemeKontrolu("insan3.png");
    insan4png = ResimYuklemeKontrolu("insan4.png");
    ork1png = ResimYuklemeKontrolu("ork1.png");
    ork2png = ResimYuklemeKontrolu("ork2.png");
    ork3png = ResimYuklemeKontrolu("ork3.png");
    ork4png = ResimYuklemeKontrolu("ork4.png");
}

Texture2D karaktere_gore_resim_dondur(const char *birim_adi)
{
    Texture2D resim = boslukResmi;

    if (strcmp(birim_adi, "kahraman") == 0)
    {
        resim = kahramanResmi;
    }
    else if (strcmp(birim_adi, "canavar") == 0)
    {
        resim = canavarResmi;
    }
    else if (strcmp(birim_adi, "piyadeler") == 0)
    {
        resim = insan1png; 
    }
    else if (strcmp(birim_adi, "okcular") == 0)
    {
        resim = insan2png;
    }
    else if (strcmp(birim_adi, "suvariler") == 0)
    {
        resim = insan3png; 
    }
    else if (strcmp(birim_adi, "kusatma_makineleri") == 0)
    {
        resim = insan4png;
    }
    else if (strcmp(birim_adi, "ork_dovusculeri") == 0)
    {
        resim = ork1png;
    }
    else if (strcmp(birim_adi, "mizrakcilar") == 0)
    {
        resim = ork2png; 
    }
    else if (strcmp(birim_adi, "varg_binicileri") == 0)
    {
        resim = ork3png; 
    }
    else if (strcmp(birim_adi, "troller") == 0)
    {
        resim = ork4png;
    }

    return resim; 
}

void izgara_baslat()
{
    for (int i = 0; i < IZGARA; i++)
    {
        for (int j = 0; j < IZGARA; j++)
        {
            strcpy(grid[i][j].type, "empty");
            grid[i][j].count = 0;
            grid[i][j].texture = boslukResmi;
            strcpy(grid[i][j].name, "");
        }
    }
}

void birimleri_yerlestir(const char *birim_turu, int sayi, Texture2D resim, int durduguYon)
{
    int yerlesen = 0;
    int start_sutun = (durduguYon == 0) ? 0 : IZGARA / 2;
    int end_sutun = (durduguYon == 0) ? IZGARA / 2 : IZGARA;

    while (yerlesen < sayi)
    {
        int i = rand() % IZGARA;
        int j = start_sutun + (rand() % (end_sutun - start_sutun));

        if (strcmp(grid[i][j].type, "empty") == 0)
        {
            strcpy(grid[i][j].type, birim_turu);
            grid[i][j].count = (sayi - yerlesen > HUCRE_MBIRIM) ? HUCRE_MBIRIM : (sayi - yerlesen);
            grid[i][j].texture = resim;
            yerlesen += grid[i][j].count;
        }
    }
    printf("%d %s, %d tarafina yerlestirildi\n", yerlesen, birim_turu, durduguYon);
}

void kahraman_ve_canavar_yerlestir(const char *birim_turu, Taraf *taraf, Texture2D resim, int side)
{
    int start_sutun = (side == 0) ? 0 : IZGARA / 2;
    int end_sutun = (side == 0) ? IZGARA / 2 : IZGARA;
    int sayi = (strcmp(birim_turu, "kahraman") == 0) ? taraf->hero_count : taraf->monster_count;   
    char(*units)[MDEGER] = (strcmp(birim_turu, "kahraman") == 0) ? taraf->hero : taraf->monster; 

    for (int i = 0; i < sayi; i++)
    {
        int deneme = 0;
        while (deneme < 100) 
        {
            int satir = rand() % IZGARA;
            int sutun = start_sutun + (rand() % (end_sutun - start_sutun));

            if (strcmp(grid[satir][sutun].type, "empty") == 0)
            {
                strcpy(grid[satir][sutun].type, birim_turu);
                strcpy(grid[satir][sutun].name, units[i]);
                grid[satir][sutun].count = 1;
                grid[satir][sutun].texture = resim;
                printf(" %s (%s) , [%d][%d] konumunda %d tarafina yerlestirildi.\n", birim_turu, units[i], satir, sutun, side);
                break;
            }
            deneme++;
        }
        if (deneme >= 100)
        {
            printf(" %s (%s), %d tarafina yerlestirilemedi.\n", birim_turu, units[i], side);
        }
    }
}

void resim1()
{
    DrawTexturePro(arkaplanResmi,
                   (Rectangle){0, 0, arkaplanResmi.width, arkaplanResmi.height},
                   (Rectangle){0, 0, ALAN_G, ALAN_Y},
                   (Vector2){0, 0}, 0, WHITE);

    for (int i = 0; i < IZGARA; i++)
    {
        for (int j = 0; j < IZGARA; j++)
        {
            Rectangle hucre = (Rectangle){j * HUCRE, i * HUCRE, HUCRE, HUCRE};
            DrawRectangleLinesEx(hucre, 1, GRAY);

            if (strcmp(grid[i][j].type, "empty") != 0)
            {
                DrawTexture(grid[i][j].texture,
                            hucre.x + (HUCRE - grid[i][j].texture.width) / 2,
                            hucre.y + (HUCRE - grid[i][j].texture.height) / 2,
                            WHITE);

                const char *adi_goster = grid[i][j].type;
                if (strcmp(grid[i][j].type, "kahraman") == 0 || strcmp(grid[i][j].type, "canavar") == 0)
                {
                    adi_goster = grid[i][j].name;
                }

                int metin_genisligi = MeasureText(adi_goster, 10);
                int metinX = hucre.x + (HUCRE - metin_genisligi) / 2;
                int metinY = hucre.y + HUCRE - 15;

                DrawRectangle(metinX - 2, metinY - 2, metin_genisligi + 4, 14, (Color){0, 0, 0, 180});
                DrawText(adi_goster, metinX, metinY, 10, WHITE);

                if (strcmp(grid[i][j].type, "kahraman") != 0 && strcmp(grid[i][j].type, "canavar") != 0)
                {

                    float saglik_yuzdesi = 100.0f;                                       
                    float saglik_bari_genislik = (HUCRE - 10) * (saglik_yuzdesi / 100.0f); 

                    DrawRectangle(hucre.x + 5, hucre.y + HUCRE - 25, HUCRE - 10, 5, DARKGRAY);
                    DrawRectangle(hucre.x + 5, hucre.y + HUCRE - 25, saglik_bari_genislik, 5, GREEN); 

                    char saglik_bari_metin[10];
                    snprintf(saglik_bari_metin, sizeof(saglik_bari_metin), "%.0f%%", saglik_yuzdesi);
                    int saglik_bari_metin_genislik = MeasureText(saglik_bari_metin, 10);
                    DrawText(saglik_bari_metin, hucre.x + (HUCRE - saglik_bari_metin_genislik) / 2, hucre.y + HUCRE - 35, 10, WHITE); 
                }
            }
        }
    }
}


void dizi_karistir(int *dizi, int boyut)
{
    if (boyut > 1)
    {
        for (int i = boyut - 1; i > 0; i--)
        {
            int j = rand() % (i + 1);
            int temp = dizi[i];
            dizi[i] = dizi[j];
            dizi[j] = temp;
        }
    }
}

void resim2()
{
    static int kontrol = 0;

    if (!kontrol)
    {
        kontrol = 1; 

        for (int i = 0; i < IZGARA; i++)
        {
            for (int j = 0; j < IZGARA; j++)
            {
                strcpy(grid[i][j].type, "empty");
                grid[i][j].count = 0;
                grid[i][j].texture = boslukResmi;
                strcpy(grid[i][j].name, "");
            }
        }

        Taraf *kazananTaraf = (ilk_struct.insan_imparatorlugu.character_count > 0) ? &ilk_struct.insan_imparatorlugu : &ilk_struct.ork_legi;

        int yasayan_sayisi = 0;
        for (int i = 0; i < kazananTaraf->character_count; i++)
        {
            if (kazananTaraf->characters[i].health > 0)
            {
                yasayan_sayisi++;
            }
        }

        if (yasayan_sayisi == 0)
        {
            const char *yasayanYok = "Yasayan yok!";
            int metin_genisligi = MeasureText(yasayanYok, 20);
            DrawText(yasayanYok, (ALAN_G - metin_genisligi) / 2, ALAN_Y / 2, 20, WHITE);
            return;
        }

        int toplam_hucre = IZGARA * IZGARA;
        int *hucre_indexler = (int *)malloc(toplam_hucre * sizeof(int));
        if (!hucre_indexler)
        {
            printf("Hucreler için bellek ayirma islemi basarisiz oldu.\n");
            return;
        }
        for (int i = 0; i < toplam_hucre; i++)
        {
            hucre_indexler[i] = i;
        }

        dizi_karistir(hucre_indexler, toplam_hucre);

        int simdiki_hucre = 0; 

        for (int i = 0; i < kazananTaraf->character_count; i++)
        {
            Karakter *yerlesecek_birim = &kazananTaraf->characters[i];


            if (yerlesecek_birim->health <= 0)
            {
                continue;
            }

            int kalan_sayisi  = yerlesecek_birim->count;
            const char *birim_turu = yerlesecek_birim->name;
            Texture2D birimResmi = karaktere_gore_resim_dondur(birim_turu);

            if (birimResmi.id == 0)
            {
            
                birimResmi = boslukResmi;
                printf(" %s icin resim bulunamadi. Bos resim kullanilacak.\n", birim_turu);
            }

            while (kalan_sayisi > 0 && simdiki_hucre < toplam_hucre)
            {
                int hucre_indeks = hucre_indexler[simdiki_hucre];
                int satir = hucre_indeks / IZGARA;
                int sutun = hucre_indeks % IZGARA;

                if (strcmp(grid[satir][sutun].type, "empty") == 0 || strcmp(grid[satir][sutun].type, birim_turu) == 0)
                {
                    int kalan_yer = (strcmp(grid[satir][sutun].type, "empty") == 0) ? HUCRE_MBIRIM : (HUCRE_MBIRIM - grid[satir][sutun].count);
                    if (kalan_yer <= 0)
                    {
                        simdiki_hucre++;
                        continue; 
                    }

                    int yerlestir = (kalan_sayisi > kalan_yer) ? kalan_yer : kalan_sayisi;

                    strcpy(grid[satir][sutun].type, birim_turu);
                    grid[satir][sutun].count += yerlestir;
                    grid[satir][sutun].texture = birimResmi;
                    strcpy(grid[satir][sutun].name, birim_turu); 

                    kalan_sayisi -= yerlestir;
                    printf(" %d adet  %s birimi [%d][%d] hucresine yerlesti.\n", yerlestir, birim_turu, satir, sutun);
                }

                simdiki_hucre++;
            }

            if (kalan_sayisi > 0)
            {
                printf(" %s biriminin tumunu yerlestirmek icin yeterli hucre yok! Kalan sayisi: %d\n", birim_turu, kalan_sayisi);
            }
        }

        free(hucre_indexler); 
    }

    DrawTexturePro(arkaplanResmi,
                   (Rectangle){0, 0, arkaplanResmi.width, arkaplanResmi.height},
                   (Rectangle){0, 0, ALAN_G, ALAN_Y},
                   (Vector2){0, 0}, 0, WHITE);

    for (int i = 0; i < IZGARA; i++)
    {
        for (int j = 0; j < IZGARA; j++)
        {
            Rectangle hucre = (Rectangle){j * HUCRE, i * HUCRE, HUCRE, HUCRE};
            DrawRectangleLinesEx(hucre, 1, GRAY);

            if (strcmp(grid[i][j].type, "empty") != 0)
            {
                Texture2D birimResmi = grid[i][j].texture;
                DrawTexture(birimResmi,
                            hucre.x + (HUCRE - birimResmi.width) / 2,
                            hucre.y + (HUCRE - birimResmi.height) / 2,
                            WHITE);

                const char *displayName = grid[i][j].type;
                if (strcmp(grid[i][j].type, "kahraman") == 0 || strcmp(grid[i][j].type, "canavar") == 0)
                {
                    displayName = grid[i][j].name;
                }

                int metin_genisligi = MeasureText(displayName, 10);
                int textX = hucre.x + (HUCRE - metin_genisligi) / 2;
                int textY = hucre.y + HUCRE - 15;

                DrawRectangle(textX - 2, textY - 2, metin_genisligi + 4, 14, (Color){0, 0, 0, 180});
                DrawText(displayName, textX, textY, 10, WHITE);

                if (strcmp(grid[i][j].type, "kahraman") != 0 && strcmp(grid[i][j].type, "canavar") != 0)
                {
                   
                    float max_saglik = 100.0f;     
                    float current_saglik = 100.0f; 

                    Taraf *kazananTaraf = (ilk_struct.insan_imparatorlugu.character_count > 0) ? &ilk_struct.insan_imparatorlugu : &ilk_struct.ork_legi;

                    for (int k = 0; k < kazananTaraf->character_count; k++)
                    {
                        if (strcmp(kazananTaraf->characters[k].name, displayName) == 0)
                        {
                            max_saglik = (float)kazananTaraf->characters[k].max_health;
                            current_saglik = (float)kazananTaraf->characters[k].health;
                            break;
                        }
                    }

                    float saglikYuzdesi = (max_saglik > 0.0f) ? (current_saglik / max_saglik) * 100.0f : 0.0f;
                    if (saglikYuzdesi < 0.0f)
                        saglikYuzdesi = 0.0f;
                    if (saglikYuzdesi > 100.0f)
                        saglikYuzdesi = 100.0f;

                    float saglik_bari_genisligi = (HUCRE - 10) * (saglikYuzdesi / 100.0f);

                    
                    Color bar_rengi;
                    if (saglikYuzdesi >= 80.0f)
                        bar_rengi = GREEN;
                    else if (saglikYuzdesi < 80.0f && saglikYuzdesi >= 20.0f )
                    {
                       bar_rengi = YELLOW;
                    }
                    
                    else if (saglikYuzdesi < 20.0f && saglikYuzdesi >= 0.1f )
                        bar_rengi = RED;
                    else
                        bar_rengi = DARKGRAY;

                    DrawRectangle(hucre.x + 5, hucre.y + HUCRE - 25, HUCRE - 10, 5, DARKGRAY); 
                    DrawRectangle(hucre.x + 5, hucre.y + HUCRE - 25, saglik_bari_genisligi, 5, bar_rengi); 

                    char saglikMetni[10];
                    snprintf(saglikMetni, sizeof(saglikMetni), "%.0f%%", saglikYuzdesi);
                    int saglik_bari_metin_genisligi = MeasureText(saglikMetni, 10);
                    DrawText(saglikMetni, hucre.x + (HUCRE - saglik_bari_metin_genisligi) / 2, hucre.y + HUCRE - 35, 10, WHITE); 
                }
            }
        }
    }
}

int main()
{
    savas_sim = fopen("savas_sim.txt", "w");
    if (savas_sim == NULL) {
        printf("Dosya acilamadi!\n");
        return 1;
    }

    dosya_indirme();

    char *dosya_ptr;
    char dosya_adi[] = "senaryo.json";

    dosya_ptr = dosya_oku(dosya_adi);
    if (dosya_ptr)
    {
        memset(&ilk_struct, 0, sizeof(OyunYapisi));

        json_taraf_ayristirma(dosya_ptr);
        free(dosya_ptr);
    }

    dosya_ptr = dosya_oku("creatures.json");
    if (dosya_ptr)
    {
        creatures_json_ayristirma(dosya_ptr);
        free(dosya_ptr);
    }


    dosya_ptr = dosya_oku("heroes.json");
    if (dosya_ptr)
    {
        heroes_json_ayristirma(dosya_ptr);
        free(dosya_ptr);
    }

    dosya_ptr = dosya_oku("research.json");
    if (dosya_ptr)
    {
        research_json_ayristir(dosya_ptr);
        free(dosya_ptr);
    }

    dosya_ptr = dosya_oku("unit_types.json");
    if (dosya_ptr)
    {
        unit_types_json_ayristir(dosya_ptr);
        free(dosya_ptr);
    }

    savas_ana_fonksiyon(&ilk_struct);

    fclose(savas_sim);

    InitWindow(ALAN_G, ALAN_Y, "KANLI BIR SAVAS: ORK VS. INSAN");
    SetTargetFPS(60);

    resim_yukle();
    izgara_baslat();

    // İnsan İmparatorluğu birimlerini sol tarafa yerleştir 
    birimleri_yerlestir("piyadeler", ilk_struct.insan_imparatorlugu.piyadeler, insan1png, 0);
    birimleri_yerlestir("okcular", ilk_struct.insan_imparatorlugu.okcular, insan2png, 0);
    birimleri_yerlestir("suvariler", ilk_struct.insan_imparatorlugu.suvariler, insan3png, 0);
    birimleri_yerlestir("kusatma_makineleri", ilk_struct.insan_imparatorlugu.kusatma_makineleri, insan4png, 0);

    // Ork Lejyonu birimlerini sağ tarafa yerleştir
    birimleri_yerlestir("ork_dovusculeri", ilk_struct.ork_legi.ork_dovusculeri, ork1png, 1);
    birimleri_yerlestir("mizrakcilar", ilk_struct.ork_legi.mizrakcilar, ork2png, 1);
    birimleri_yerlestir("varg_binicileri", ilk_struct.ork_legi.varg_binicileri, ork3png, 1);
    birimleri_yerlestir("troller", ilk_struct.ork_legi.troller, ork4png, 1);

    // Kahraman ve canavarları yerleştir
    kahraman_ve_canavar_yerlestir("kahraman", &ilk_struct.insan_imparatorlugu, kahramanResmi, 0);
    kahraman_ve_canavar_yerlestir("kahraman", &ilk_struct.ork_legi, kahramanResmi, 1);
    kahraman_ve_canavar_yerlestir("canavar", &ilk_struct.insan_imparatorlugu, canavarResmi, 0);
    kahraman_ve_canavar_yerlestir("canavar", &ilk_struct.ork_legi, canavarResmi, 1);

    for (int i = 0; i < IZGARA; i++)
    {
        for (int j = 0; j < IZGARA; j++)
        {
            if (strcmp(grid[i][j].type, "empty") != 0)
            {
                printf("Hucre [%d][%d]: Tur: %s, Sayi: %d", i, j, grid[i][j].type, grid[i][j].count);
                if (strcmp(grid[i][j].type, "kahraman") == 0 || strcmp(grid[i][j].type, "canavar") == 0)
                {
                    printf(", Ad: %s", grid[i][j].name);
                }
                printf("\n");
            }
        }
    }

    int simdiki_sahne = 1; 
    while (!WindowShouldClose())
    {
        while (!WindowShouldClose())
        {
            if (IsKeyPressed(KEY_ENTER))
            {
                simdiki_sahne = 2;
            }

            BeginDrawing();

            if (simdiki_sahne == 1)
            {
                resim1();
            }
            else if (simdiki_sahne == 2)
            {
                resim2();
            }

            EndDrawing();
        }

        UnloadTexture(arkaplanResmi);
        UnloadTexture(boslukResmi);
        UnloadTexture(kahramanResmi);
        UnloadTexture(canavarResmi);
        UnloadTexture(insan1png);
        UnloadTexture(insan2png);
        UnloadTexture(insan3png);
        UnloadTexture(insan4png);
        UnloadTexture(ork1png);
        UnloadTexture(ork2png);
        UnloadTexture(ork3png);
        UnloadTexture(ork4png);
        CloseWindow();
    }



    return 0;
}