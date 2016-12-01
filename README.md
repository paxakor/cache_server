# Cache Web-Server
Задача: написать кэширующий веб-сервер, способный либо перенаправлять запросы на другие ресурсы, либо выдавать кэшированный ответ.

### Сборка
Проект использует библотеку для работы с файловой системой. Если Ваш компилятор не поддерживает std::filesystem, которая войдёт в стандарт C++17 и уже давно существует как std::experimental::filesystem, то есть возможность использовать boost/filesystem. Для этого добавьте к cmake флаг<br />
`-DUSE_BOOST_FILESYSTEM:BOOL=ON`

### Автор
Корозевцев Павел<br />
email: paxakor@yandex.ru<br />
[vkontakte](vk.com/paxakor)<br />
[github](github.com/paxakor)
