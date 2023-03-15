# OpenGLDemos

## IMPORTANT NOTE !!!

**FILE HAS TO BE EXECUTED FROM build/ DUE TO RESSOURCES ACCESS**
**Resizing is kind of broken for the moment**

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

## Keybinds:

Shaders:

- glTF 2.0 microfacets
- modified microfacets
- texture microfacets
- simple lambert
- normals
- UV coords

Demos:

- **0** : clear
- **1** : Hello triangle
- **2** : Bezier Surface
- **3** : Hello Sphere
- **4** : Hello Bloom 

## Results: Bezier Surface

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

Normals

![alt text](oldResults/normals.png)

Parametric

![alt text](oldResults/parametric.png)

Gui

![alt text](oldResults/gui.png)
