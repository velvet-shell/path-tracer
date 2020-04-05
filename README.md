# Path Tracing
Monte-Carlo path tracing image rendering


### TODO:

* Explicit light sampling
* Lens and depth of field support for realistic pictures

### Implemented features:

* Positionable camera class
* Sphere, plane and triangle primitives
* Textures and spherical environment map
* Glass and mirror materials
* Lambertian BRDF (importance sampling cosine)
* Modified Phong BRDF (normalized according to Lafortune and Williams 1994, importance sampling powered cosine)
* Microfacet Cook Torrance BRDF (importance sampling GGX NDF, uncorrelated Smith+GGX masking-shadowing function)
* SSAO via tent filter (based on smallpt implementation: inverse transform sampling random values from filter)
