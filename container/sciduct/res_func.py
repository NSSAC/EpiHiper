    import math
    def get_pop_size(pop):
        total=None
        facets = pop['autometa'].get('facets',[])
        for facet in facets:
            if facet.get('ann:id',None) == 'total':
                total = facet.get('value')
                break
            continue
        if total is None:
            raise Exception('Unable to determin population size')
        return total

    population=None
    for file in job.get('input_files',[]):
        if file['name'] == 'personTraitDB':
            population = file
            break
        continue

    pop_size = get_pop_size(population)
    cores = math.ceil(pop_size / 240000)
    return {
       'capabilities': jobdef['resource_requirements'].get('capabilities',{"mpi": True}),
       'cores': cores,
       'tasks': cores,
       'timelimit': jobdef['resource_requirements']['timelimit']
    }
