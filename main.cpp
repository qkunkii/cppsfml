#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <algorithm>

struct Obstacle {
    sf::Vector2f position;
    float radius;
};

int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 800), "Variant 23: Ball on Inclined Plane with Obstacles");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("../resources/fonts/arial.ttf")) {
    }

    sf::Text infoText;
    infoText.setFont(font);
    infoText.setCharacterSize(18);
    infoText.setFillColor(sf::Color::White);
    infoText.setPosition(20.f, 20.f);

    float ballRadius = 15.f;
    sf::Vector2f ballPos(100.f, 150.f);
    sf::Vector2f ballVel(0.f, 0.f);
    
    float gravity = 500.f;          
    float friction = 0.05f;         
    float restitution = 0.7f;       
    float slopeAngleDeg = 25.f;     

    bool isRunning = true;          

    sf::RectangleShape slope(sf::Vector2f(900.f, 20.f));
    slope.setFillColor(sf::Color(120, 120, 120));
    slope.setOrigin(0.f, 0.f); 
    slope.setPosition(50.f, 200.f);

    struct ObstacleData {
        float x;
        float radius;
    };

    std::vector<ObstacleData> obstacleData = {
        { 300.f, 10.f },
        { 500.f, 10.f },
        { 700.f, 10.f }
    };

    sf::CircleShape ball(ballRadius);
    ball.setOrigin(ballRadius, ballRadius);
    ball.setFillColor(sf::Color::Green);

    std::vector<sf::Vector2f> trajectory;
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up) {
                    slopeAngleDeg += 2.f; 
                }
                if (event.key.code == sf::Keyboard::Down) {
                    slopeAngleDeg -= 2.f; 
                }
                if (event.key.code == sf::Keyboard::Right) {
                    friction += 0.01f;    
                }
                if (event.key.code == sf::Keyboard::Left) {
                    friction -= 0.01f;    
                }
                if (event.key.code == sf::Keyboard::Space) {
                    isRunning = !isRunning; 
                }
                if (event.key.code == sf::Keyboard::R) {
                    ballPos = sf::Vector2f(100.f, 150.f);
                    ballVel = sf::Vector2f(0.f, 0.f);
                    trajectory.clear();
                }
            }
        }

        slopeAngleDeg = std::clamp(slopeAngleDeg, 5.f, 60.f);
        friction = std::clamp(friction, 0.0f, 0.5f);

        float dt = clock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f; 

        if (isRunning) {
            float rad = slopeAngleDeg * 3.14159265f / 180.f;
            float startX = 50.f;
            float startY = 200.f;

            std::vector<Obstacle> obstacles;
            for (const auto& data : obstacleData) {
                float surfaceY = startY + (data.x - startX) * std::tan(rad);
                obstacles.push_back({ sf::Vector2f(data.x, surfaceY - data.radius), data.radius });
            }

            float accParallel = gravity * std::sin(rad);
            float frictionForce = friction * gravity * std::cos(rad);
            float netAccParallel = accParallel - frictionForce;
            if (netAccParallel < 0.f) netAccParallel = 0.f;

            ballVel.x += netAccParallel * std::cos(rad) * dt;
            ballVel.y += (gravity + netAccParallel * std::sin(rad)) * dt;

            ballPos += ballVel * dt;

            float expectedY = startY + (ballPos.x - startX) * std::tan(rad);
            
            if (ballPos.y + ballRadius >= expectedY && ballPos.x >= startX && ballPos.x <= startX + 900.f) {
                ballPos.y = expectedY - ballRadius; 
                ballVel.y = -ballVel.y * restitution;
                ballVel.x = ballVel.x * (1.0f - friction);
            }

            for (const auto& obs : obstacles) {
                sf::Vector2f delta = ballPos - obs.position;
                float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
                float minDist = ballRadius + obs.radius;

                if (distance < minDist) {
                    sf::Vector2f n = delta / distance;
                    ballPos = obs.position + n * minDist;

                    float dotProduct = ballVel.x * n.x + ballVel.y * n.y;
                    ballVel = (ballVel - 2.f * dotProduct * n) * restitution;
                }
            }

            trajectory.push_back(ballPos);
            if (trajectory.size() > 400) {
                trajectory.erase(trajectory.begin());
            }
        }

        ball.setPosition(ballPos);

        infoText.setString(
            "=== VARIANT 23: Ball on Inclined Plane ===\n"
            "[Up/Down] Slope Angle: " + std::to_string(slopeAngleDeg) + " deg\n" +
            "[Left/Right] Friction: " + std::to_string(friction) + "\n" +
            "[Space] Pause / Resume (Status: " + (isRunning ? "Running" : "Paused") + ")\n" +
            "[R] Reset Position"
        );

        window.clear(sf::Color(30, 30, 40));

        slope.setRotation(slopeAngleDeg);
        window.draw(slope);

        float radDraw = slopeAngleDeg * 3.14159265f / 180.f;
        for (const auto& data : obstacleData) {
            float surfaceY = 200.f + (data.x - 50.f) * std::tan(radDraw);
            sf::CircleShape obsShape(data.radius);
            obsShape.setOrigin(data.radius, data.radius);
            obsShape.setPosition(sf::Vector2f(data.x, surfaceY - data.radius));
            obsShape.setFillColor(sf::Color(200, 100, 100));
            window.draw(obsShape);
        }

        if (trajectory.size() > 1) {
            sf::VertexArray lines(sf::PrimitiveType::LineStrip, trajectory.size());
            for (size_t i = 0; i < trajectory.size(); ++i) {
                lines[i] = sf::Vertex(trajectory[i], sf::Color(200, 200, 200, 150));
            }
            window.draw(lines);
        }

        window.draw(ball);
        window.draw(infoText);

        window.display();
    }

    return 0;
}