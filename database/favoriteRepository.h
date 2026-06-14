#pragma once
#include <QList>

class FavoriteRepository {
public:
    static bool addFavorite(int userId, int starId);
    static bool removeFavorite(int userId, int starId);
    static QList<int> getFavorites(int userId);
    static bool isFavorite(int userId, int starId);
};
