#include "Utils.h"

void pascalCoeffs(int n, std::vector<std::vector<int>>& coeffs){
    for(int line = 1; line <= n; line++){
        int C = 1;
        std::vector<int> coeffI;
        for(int i = 1; i <= line; i++){
            coeffI.push_back(C);
            C = C * (line - i) / i;
        }
        coeffs.push_back(coeffI);
    }
}

glm::vec3 bezierValue(std::vector<glm::vec3>& p, float t, std::vector<std::vector<int>>& coeffs){
    glm::vec3 pu(0.0f);
    int n = p.size()-1;
    for(int i = 0; i <= n; i++){
        float Bi = coeffs[n][i] * powf(t, i) * powf(1.0f-t, n-i);  
        pu += Bi * p[i];
    }
    return pu;
}
