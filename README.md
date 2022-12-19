# OpenGLDemos

## IMPORTANT NOTE !!!

**FILE HAS TO BE EXECUTED FROM build/ DUE TO RESSOURCES ACCESS LIKE Shaders/ AND Assets/**

## Getting Started

```bash
git clone https://github.com/Mafo369/OpenGLDemos.git
git checkout mesh
git submodule update --init --recursive
```

```bash
mkdir build
cd build
cmake ..
make
./OpenGLDemos
```

## Keybinds:

Shaders:

- **c** : glTF 2.0 microfacets
- **m** : modified microfacets
- **t** : texture microfacets
- **l** : simple lambert
- **n** : normals
- **u** : UV coords

Demos:

- **0** : clear
- **1** : Hello triangle
- **2** : Bezier Surface
- **3** : Hello Sphere
- **4** : Hello Bunny 
- **5** : Hello Monkey

## Results: Bezier Surface

Microfacets

![alt text](results/microfacets.png)

Microfacets modified

![alt text](results/microfacetsmodified.png)

Microfacets + texture

![alt text](results/microfacetstexture.png)

Microfacets modified

![alt text](results/microfacetsmodified1.png)

Lambert

![alt text](results/lambert.png)

Normals

![alt text](results/normals.png)

Parametric

![alt text](results/parametric.png)

Gui

![alt text](results/gui.png)
