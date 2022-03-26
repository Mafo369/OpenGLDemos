#include <vector>
#include <glm/glm.hpp>

void pascalCoeffs(int n, std::vector<std::vector<int>>& coeffs);

glm::vec3 bezierValue(std::vector<glm::vec3>& p, float t, std::vector<std::vector<int>>& coeffs);
