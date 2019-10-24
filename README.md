![Tripal Dependency](https://img.shields.io/badge/tripal-%3E=3.0-brightgreen)
![Module is Generic](https://img.shields.io/badge/generic-tested%20manually-yellow)
![GitHub release (latest by date including pre-releases)](https://img.shields.io/github/v/release/UofS-Pulse-Binfo/tripal_qtl?include_prereleases)

[![Build Status](https://travis-ci.org/UofS-Pulse-Binfo/tripal_qtl.svg?branch=master)](https://travis-ci.org/UofS-Pulse-Binfo/tripal_qtl)
[![Maintainability](https://api.codeclimate.com/v1/badges/db8bad906e18da15382e/maintainability)](https://codeclimate.com/github/UofS-Pulse-Binfo/tripal_genetic/maintainability)
[![Test Coverage](https://api.codeclimate.com/v1/badges/db8bad906e18da15382e/test_coverage)](https://codeclimate.com/github/UofS-Pulse-Binfo/tripal_qtl/test_coverage)

# Tripal QTL (Tripal 3)

This module provides additional fields and data import which provide support for quantitative trait loci. Additionally, it integrates with Tripal Map through shared vocabulary terms and data storage models.

## Installation

1. Install this module as you would any other Drupal module.
2. (Recommended) Install Tripal Map: https://gitlab.com/mainlabwsu/tripal_map

## Usage

1. Load your genetic map using the MST Map Importer and then load the associated QTL using the QTL importer.
2. Go to Admin > Tripal > Data Storage > Chado > Materialized Views and populate `tripal_map_genetic_markers_mview` and `tripal_map_qtl_and_mtl_mview`. This will make your data available to Tripal Map and the fields provided to this module.
3. Go to the Tripal Map Quickstart at `http://localhost/MapViewer` and check your map out!
4. Create genetic map pages by going to Admin > Content > Tripal Content, click "Publish" and choose "Genetic Map" from the drop-down.

## Importers
 - [Genetic Maps (MSTmap Format)](https://tripal-map-helper.readthedocs.io/en/latest/MSTmap.html)
 - Genetic Markers & Loci: COMING SOON
 - Quantitative Trait Loci (QTL): COMING SOON

## Documentation [![Documentation Status](https://readthedocs.org/projects/tripal-map-helper/badge/?version=latest)](https://tripal-map-helper.readthedocs.io/en/latest/?badge=latest)

We have extensive documentation for all our available importers including form screenshots, validation and data storage ER diagrams on [ReadtheDocs](https://tripal-map-helper.readthedocs.io/en/latest/).
