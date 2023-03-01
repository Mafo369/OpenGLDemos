#include "Cube.h"
#include <GL/gl.h>

Cube::Cube(glm::vec4 color) : Mesh() {
    m_color = color;
    m_vertices = {
        // back face
      {    {-1.0f, -1.0f, -1.0f},  {0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}, m_color}, // bottom-left
      {    { 1.0f,  1.0f, -1.0f},  {0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}, m_color}, // top-right
      {    { 1.0f, -1.0f, -1.0f},  {0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}, m_color}, // bottom-right         
      {    { 1.0f,  1.0f, -1.0f},  {0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}, m_color}, // top-right
      {    {-1.0f, -1.0f, -1.0f},  {0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}, m_color}, // bottom-left
      {    {-1.0f,  1.0f, -1.0f},  {0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}, m_color}, // top-left
          // front face                                       
      {    {-1.0f, -1.0f,  1.0f},  {0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}, m_color}, // bottom-left
      {    { 1.0f, -1.0f,  1.0f},  {0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}, m_color}, // bottom-right
      {    { 1.0f,  1.0f,  1.0f},  {0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}, m_color}, // top-right
      {    { 1.0f,  1.0f,  1.0f},  {0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}, m_color}, // top-right
      {    {-1.0f,  1.0f,  1.0f},  {0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}, m_color}, // top-left
      {    {-1.0f, -1.0f,  1.0f},  {0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}, m_color}, // bottom-left
          // left face                                      
      {    {-1.0f,  1.0f,  1.0f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}, m_color}, // top-right
      {    {-1.0f,  1.0f, -1.0f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}, m_color}, // top-left
      {    {-1.0f, -1.0f, -1.0f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}, m_color}, // bottom-left
      {    {-1.0f, -1.0f, -1.0f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}, m_color}, // bottom-left
      {    {-1.0f, -1.0f,  1.0f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}, m_color}, // bottom-right
      {    {-1.0f,  1.0f,  1.0f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}, m_color}, // top-right
          // right face                                     
      {     {1.0f,  1.0f,  1.0f},  {1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}, m_color}, // top-left
      {     {1.0f, -1.0f, -1.0f},  {1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}, m_color}, // bottom-right
      {     {1.0f,  1.0f, -1.0f},  {1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}, m_color}, // top-right         
      {     {1.0f, -1.0f, -1.0f},  {1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}, m_color}, // bottom-right
      {     {1.0f,  1.0f,  1.0f},  {1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}, m_color}, // top-left
      {     {1.0f, -1.0f,  1.0f},  {1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}, m_color}, // bottom-left     
          // bottom face                                   
      {    {-1.0f, -1.0f, -1.0f},  {0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}, m_color}, // top-right
      {    { 1.0f, -1.0f, -1.0f},  {0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}, m_color}, // top-left
      {    { 1.0f, -1.0f,  1.0f},  {0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}, m_color}, // bottom-left
      {    { 1.0f, -1.0f,  1.0f},  {0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}, m_color}, // bottom-left
      {    {-1.0f, -1.0f,  1.0f},  {0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}, m_color}, // bottom-right
      {    {-1.0f, -1.0f, -1.0f},  {0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}, m_color}, // top-right
          // top face                                     
      {    {-1.0f,  1.0f, -1.0f},  {0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}, m_color}, // top-left
      {    { 1.0f,  1.0f , 1.0f},  {0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}, m_color}, // bottom-right
      {    { 1.0f,  1.0f, -1.0f},  {0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}, m_color}, // top-right     
      {    { 1.0f,  1.0f,  1.0f},  {0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}, m_color}, // bottom-right
      {    {-1.0f,  1.0f, -1.0f},  {0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}, m_color}, // top-left
      {    {-1.0f,  1.0f,  1.0f},  {0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}, m_color}  // bottom-left        
    };  
    for(int i = 0; i < 6*6; i++){
      m_indices.push_back(i);
    }

    setupMesh(GL_TRIANGLES);
}

Cube::~Cube(){

}
