
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define c_nMaxPathLen   260     /* MAX_PATH pe Windows */
#define c_nBufferSize   4096    /* dimensiunea bufferului de citire */


int main(void)
{
    // sz = string 
    char szCaleRelativa[c_nMaxPathLen];

    // n = intreg (int) 
    int nRezultat;

    printf("Introdu path-ul fisierului: ");

    if (fgets(szCaleRelativa, c_nMaxPathLen, stdin) == NULL)
    {
        fprintf(stderr, "Eroare la calea fisierului.\n");
        return EXIT_FAILURE;
    }

    // elimina \n de la sfarsit
    {
        int nLungime = (int)strlen(szCaleRelativa);
        if (nLungime > 0 && szCaleRelativa[nLungime - 1] == '\n')
        {
            szCaleRelativa[nLungime - 1] = '\0';
        }
    }

    nRezultat = fnCitesteSiAfiseazaFisier(szCaleRelativa);

    return nRezultat;
}

int fnCitesteSiAfiseazaFisier(const char* pszCaleRelativa)
{
    // p = pointer f = FILE
    FILE* pfFisier;

    // ach = array of char
    char achBuffer[c_nBufferSize];

    // n = intreg 
    int nCaractereCtite;

    if (pszCaleRelativa == NULL || pszCaleRelativa[0] == '\0')
    {
        fprintf(stderr, "cale invalida a fisierului.\n");
        return EXIT_FAILURE;
    }

    pfFisier = fopen(pszCaleRelativa, "rb");

    if (pfFisier == NULL)
    {
        fprintf(stderr, "fisierul \"%s\" nu exista sau nu poate fi deschis.\n",
            pszCaleRelativa);
        return EXIT_FAILURE;
    }

    printf("\nContinutul fisierului \"%s\"\n\n", pszCaleRelativa);

    while ((nCaractereCtite = (int)fread(achBuffer, sizeof(char), c_nBufferSize, pfFisier)) > 0)
    {
        fwrite(achBuffer, sizeof(char), (size_t)nCaractereCtite, stdout);
    }

    if (ferror(pfFisier))
    {
        fprintf(stderr, "\neroare la citirea fisierului\n");
        fclose(pfFisier);
        return EXIT_FAILURE;
    }

    fclose(pfFisier);

    return EXIT_SUCCESS;
}
