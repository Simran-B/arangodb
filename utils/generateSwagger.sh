#!/bin/bash

python \
  C:\\Users\\Simran\\Projekte\\ArangoDB\\arangodb\\utils\\generateSwagger.py \
  C:\\Users\\Simran\\Projekte\\ArangoDB\\arangodb \
  C:\\Users\\Simran\\Projekte\\ArangoDB\\arangodb\\js\\apps\\system\\_admin\\aardvark\\APP\\api-docs \
  api-docs \
  C:\\Users\\Simran\\Projekte\\ArangoDB\\arangodb\\Documentation\\DocuBlocks\\Rest \
  > `pwd`/js/apps/system/_admin/aardvark/APP/api-docs.json
