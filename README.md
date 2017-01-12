Serwer:

oczekuje na połączenia UDP na porcie 8796,
jeśli otrzyma od klienta komunikat tekstowy "czas" wysyła (tekstowo) informacje o bieżącym czasie systemowym,
w przeciwnym wypadku wysyła komunikat "blad zapytania".

Klient:

wymaga jednego parametru określającego adres IPv4 serwera (np. pętla zwrotna: 127.0.0.1),
wysyła do serwera żądanie "czas",
wyświetla odpowiedź.
Punktacja:

wersja podstawowa: 12 punktów,
serwer, który przechodzi w tło (staje się demonem) i nie zapisuje nigdzie logów +6 punktów,
serwer używający dwóch protokołów: TCP oraz UDP oraz klient w którym przy pomocy dodatkowego parametru (np. -t oraz -u), wybieramy protokół używany przez klienta +6 punktów.
