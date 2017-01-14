Serwer działa w tle, aby zakończyć jego działanie należy wysłać sygnał z lini poleceń. Serwer obsługuje zarówno protokół TCP jak i UDP.

Klient wymaga podania jako pierwszego argumentu adresu serwera i drugiego, określającego protokół (-t dla TCP lub -u dla UDP). Po uruchomieniu klient pobiera od użytkownika wiadomość, która zostanie wysłana do serwera, a następnie wyświetla na ekranie wiadomość zwrotną, którą jest czas serwera, jeśli tręścią był "czas", lub w przeciwnym wypadku "błąd zapytania". Po wyświetleniu wiadomości zwrotnej program klienta kończy swoje działanie.
