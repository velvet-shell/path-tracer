# Path Tracing
Monte-Carlo path tracing image rendering


### TODO:

* Refraction based on Fresnel equations
* Explicit light sampling
* Camera class
* Lens and depth of field support for realistic pictures
* Add more geometric primitives

### Implemented features:

* Sphere primitive
* Lambertian BRDF (importance sampling cosine)
* Modified Phong BRDF (normalized according to Lafortune and Williams 1994, importance sampling powered cosine)
* Microfacet Cook Torrance BRDF (importance sampling GGX NDF, uncorrelated Smith+GGX masking-shadowing function)
* SSAO via tent filter (based on smallpt implementation: inverse transform sampling random values from filter)
