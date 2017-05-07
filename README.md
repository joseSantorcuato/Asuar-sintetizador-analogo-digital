#README
#SINTETIZADOR ASUAR
#JOSÉ SANTORCUATO TAPIA
#santorcuato76@gmail.com
#Chile 2017

Sintetizador digital de comportamiento análogo.
Tiene posibilidad de controlar los siguientes parámetros:
</br>

lfo, forma de onda, ataque, decaimiento, sostenimiento y relajación.
</br>
Además tiene conexión midi, se puede utilizar junto a Ableton o secuenciadores, interfaces o controladores.
Se utilizó midi shield.



   onda     =  map(valorPoteDos, 0,1023, 0,10);
   at       =  map(valorPoteTres, 0, 1023, 0,1000);
   de       =  map(valorPoteCuatro, 0, 1023, 0,1000);
   sus      =   map(valorPoteCinco, 0, 1023, 0,1000);
   rea      =   map(valorPoteSeis, 0, 1023, 0,5000);
