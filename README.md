![Logo](/docs/logo.png)

CI/CD Status: [![pipeline status](https://repo.kamit.fi/ttv23sp/peliteknologian-projekti-3/projektit/tiimi1/badges/main/pipeline.svg)](https://repo.kamit.fi/ttv23sp/peliteknologian-projekti-3/projektit/tiimi1/-/commits/main)

Latest Build: http://86.50.20.238:8080

License: Tile is availaible under 2 licenses, The Happy Bunny License and the Beerware License. For more information, see [LICENSE](/docs/LICENSE.txt).

## About

Tile (lyh. **T**ile **i**s a **L**evel **E**ditor) on WebGL 2.0 ja Emscriptenin avulla toimiva monipuolinen tilemap editori.
Tilen viimeisin versio pyörii aina CSCn kautta hostatulla [remote serverillä](http://86.50.20.238:8080).
Viimeisimpä keskeneräisiä / kokeellisia buildeja voi pöyrittää omalla tietokoneella
Dockerin avulla (ohjeet alla).

Tilessä on kolme eri export vaihtoehtoa
1. Tekstitiedosto (.txt), jossa on valmis tilemap, tekstuurien nimet ja clear colorit, jotka voi Copy & Pastella laittaa projektiin
2. C++ Header (.h), jossa on samat asiat kun .txt tiedostossa, erona, että mappia ei tarvitse Copy & Pastettaa, vaan sen saa käyttöön #include -makrolla.
3. JSON, joka on tehty muistuttamaan Tiledillä tehtyjä .json tiedostoja, jotta parsimislogiikka toimii samalla tavalla.

Tämän lisäksi, keskeneräiset projektit voi tallentaa .tile muotoon. Tätä tiedostoa ei ole tehty ihmisen luettavaksi, vaan sen ideana on olla
vain ja ainoastaan läjä dataa, jonka Tile voi parsia. .tile tiedostoon tallentuu seuraavat asiat:
- [x] Tilelayerit (Nimet, TileIDt, Määrä)
- [x] Mapin koko
- [x] Clear Color
- [x] Grid Color
- [x] Tekstuurien nimet ja Määrä
- [x] Tekstuurit Base64 Encodattuna


## How To?

**Via Terminal (Dev Sandboxes)**

<ol>
    <li> docker build -t map-editor . </li>
    <li> docker run -p 8080:8080 map-editor </li>
    <li> Open: http://127.0.0.1:8080/ </li>
</ol>

**Via Terminal (Editor Only)**

<ol>
    <li> docker compose build </li>
    <li> docker compose up </li>
    <li> Open: http://127.0.0.1:10001/ </li>
</ol>

**Via Docker Desktop**

<ol>
    <li> Powershell: docker build -t map-editor . </li>
    <li> Docker Desktop -> Images -> map-editor -> Run </li>
    <li> Optional Settings -> Ports -> Set Host port to '8080' </li>
    <li> Run the Container </li>
    <li> Open: http://127.0.0.1:8080/ </li>
</ol>

## Tekijät

Julius Muurinen - [Linkki tuntikirjanpitoon](https://edukainuu-my.sharepoint.com/:x:/r/personal/juliusmuurinen_kamk_fi/Documents/Julius%20Muurinen%20-%20Projekti%203.xlsx?d=w66a3d9aa96c2439293933ebf6ee7edd6&csf=1&web=1&e=Xuy6gq)

Eetu Kuru - Linkki tuntikirjanpitoon


# Hyödyllisiä linkkejä

[CSC Projekti](https://my.csc.fi/projects/invitation/ba53167a-382e-4cf4-a0f3-61116f241845)

[Interaktiivinen Imgui manuaali, koodin kera](https://pthom.github.io/imgui_manual_online/manual/imgui_manual.html)

