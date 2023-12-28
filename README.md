# OpenGLDemos

OpenGL implementation of core 3D graphics concepts.

Features:
- HDR rendering: [Call-of-Duty:Advanced Warface Bloom implementation](https://www.iryoku.com/next-generation-post-processing-in-call-of-duty-advanced-warfare/) + basic tonemapping
- glTF 2.0 model loading and rendering
- Environment mapping
- Cascaded shadow mapping
- Microfacet material model
- Skinning animation (CPU and GPU based)
- Bezier surfaces

## Getting Started

```bash
git clone --recursive https://github.com/Mafo369/OpenGLDemos.git
```

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ../
make
./OpenGLDemos
```

## Results: Bezier Surface


Damaged Helmet

![alt text](results/helmet.png)

Bloom

![alt text](results/bloom.png)

Cascaded shadow mapping

![alt text](results/csm.png)

Skinning animation

![alt text](results/arm0.png)
![alt text](results/arm1.png)
![alt text](results/arm2.png)

Microfacets

![alt text](oldResults/microfacets.png)

Microfacets modified

![alt text](oldResults/microfacetsmodified.png)

Microfacets + texture

![alt text](oldResults/microfacetstexture.png)

Microfacets modified

![alt text](oldResults/microfacetsmodified1.png)

Lambert

![alt text](oldResults/lambert.png)

