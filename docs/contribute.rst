Contributing
==============

We’re excited to work with you! Post in the issues queue with any questions, feature requests, or proposals.

Automated Testing
--------------------

This module uses `Tripal Test Suite <https://tripaltestsuite.readthedocs.io/en/latest/installation.html#joining-an-existing-project>`_. To run tests locally:

.. code:: bash

  cd MODULE_ROOT
  composer up
  ./vendor/bin/phpunit

This will run all tests associated with the Tripal QTL extension module. If you are running into issues, this is a good way to rule out a system incompatibility.

.. warning::

  It is highly suggested you ONLY RUN TESTS ON DEVELOPMENT SITES. We have done our best to ensure that our tests clean up after themselves; however, we do not guarantee there will be no changes to your database.

Manual Testing (Demonstration)
--------------------------------

We have provided a `Tripal Test Suite Database Seeder <https://tripaltestsuite.readthedocs.io/en/latest/db-seeders.html>` to make development and demonstration of functionality easier. To populate your development database with a fake genetic map with associated QTL:

1. Install this module according to the installation instructions.
2. Create an organism (genus: Tripalus; species: databasica)
3. Run the database seeder to populate the database using the following commands:

  .. code::

    cd MODULE_ROOT
    composer up
    ./vendor/bin/tripaltest db:seed GeneticMapSeeder

4. Populate the materialized views by going to Administration » Tripal » Data Storage » Chado » Materialized Views and clicking "Populate" beside ``tripal_map_genetic_markers_mview`` and ``tripal_map_qtl_and_mtl_mview``. Finally run the Tripal jobs submitted.
5. Create a matching Genetic map page by going to Administration » Content » Tripal Content » Publish Tripal Content. Then select Genetic map from the drop down, click "Publish" and run the associated Tripal job.

  - To see the new functionality provided by this module go to Administration » Structure » Tripal Content Types » Genetic Maps » Manage Fields and click "Check for new Fields". Then go to "Manage Display" and make sure they are all visible.

.. warning::

  NEVER run database seeders on production sites. They will insert fictitious data into Chado.
