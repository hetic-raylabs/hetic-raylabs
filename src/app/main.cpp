#include <iostream>
#include "core/Camera.hpp"
#include "core/HitRecord.hpp"
#include "core/Ray.hpp"
#include "math/Vec3.hpp"
#include "entities/Sphere.hpp"

#include "image/Image.hpp"
#include "math/Color.hpp"

using namespace std;

// Fonction pour calculer la couleur d'un rayon
Color ray_color(const Ray& ray, const Sphere& sphere) {
    HitRecord rec;

    // Test d'intersection avec la sphère
    // t_min = 0.0 pour éviter les auto-intersections
    // t_max = très grand nombre (infini pratique)
    if (sphere.hit(ray, 0.0f, 1000000.0f, rec)) {
        // Couleur dorée de base (RGB: or = 1.0, 0.84, 0.0)
        float gold_r = 1.0f;
        float gold_g = 0.84f;
        float gold_b = 0.0f;

        // Calcul d'un éclairage simple basé sur la normale
        // On simule une lumière venant d'en haut à droite
        Vec3 light_direction = normalize(Vec3(1.0f, 1.0f, 1.0f));
        float light_intensity = dot(rec.normal, light_direction);

        // S'assurer que l'intensité est entre 0.2 (ombre) et 1.0 (pleine lumière)
        if (light_intensity < 0.2f) light_intensity = 0.2f;
        if (light_intensity > 1.0f) light_intensity = 1.0f;

        // Appliquer l'éclairage à la couleur dorée
        float r = gold_r * light_intensity;
        float g = gold_g * light_intensity;
        float b = gold_b * light_intensity;

        return Color(r, g, b);
    }

    // Si pas d'intersection, on affiche un gradient de fond (ciel)
    Vec3 unit_direction = normalize(ray.direction);
    float t = 0.5f * (unit_direction.y + 1.0f);

    // Interpolation linéaire entre blanc (bas) et bleu ciel (haut)
    float r = (1.0f - t) * 1.0f + t * 0.5f;
    float g = (1.0f - t) * 1.0f + t * 0.7f;
    float b = (1.0f - t) * 1.0f + t * 1.0f;

    return Color(r, g, b);
}

int main() {
    // Configuration de l'image
    const float aspect_ratio = 16.0f / 9.0f;
    const int image_width = 800;
    const int image_height = static_cast<int>(image_width / aspect_ratio);

    cout << "Rendering image of size " << image_width << "x" << image_height << endl;

    // Configuration de la caméra
    Point3 camera_pos(0, 0, 0);      // Caméra à l'origine
    Point3 look_at(0, 0, -1);        // Regarde vers -Z
    Vec3 up(0, 1, 0);                // Vecteur "haut" = Y
    float fov = 90.0f;               // Champ de vision de 90 degrés

    Camera camera(camera_pos, look_at, up, fov, aspect_ratio);

    // Création de la sphère
    // Centre à (0, 0, -1) devant la caméra, rayon de 0.5
    Sphere sphere(Point3(0, 0, -1), 0.5f);

    // Création de l'image
    Image image(image_width, image_height);

    // Rendu : pour chaque pixel, on génère un rayon et on calcule sa couleur
    cout << "Rendering..." << endl;
    for (int j = image_height - 1; j >= 0; --j) {
        if (j % 50 == 0) {
            cout << "Scanlines remaining: " << j << endl;
        }

        for (int i = 0; i < image_width; ++i) {
            // Coordonnées normalisées du pixel dans [0, 1]
            float u = float(i) / (image_width - 1);
            float v = float(j) / (image_height - 1);

            // Génération du rayon pour ce pixel
            Ray ray = camera.get_ray(u, v);

            // Calcul de la couleur
            Color pixel_color = ray_color(ray, sphere);

            image.SetPixel(i, image_height - 1 - j, pixel_color);
        }
    }

    cout << "Writing to file..." << endl;
    image.WriteFile("./output/sphere.png");

    return 0;
}
