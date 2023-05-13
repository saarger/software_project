def open_file(filename):
    vectors = []
    with open(filename, 'r') as f:
        for line in f:
            values = line.strip().split(',')
            vector = tuple([float(value) for value in values])
            vectors.append(vector)
    return vectors


def k_means(k,input_data,iter = 200):

    try:
        if not 1<iter<1000:
            print("Invalid maximum iteration!")
            return
        epsilon = 0.001
        vectors = open_file(input_data)
        if not 1 < k < len(vectors):
            print("Invalid number of clusters!")
            return
        centroids = []



        for i in range(k):
            centroids.append(vectors[i])

        i = 0
        delta_uk = 1

        while i < iter and delta_uk > epsilon:
            dic = update_dict(centroids)
            for vector in vectors:
                min_dis = float('inf')
                for center in centroids:
                    dis = d(vector,center)
                    if dis < min_dis: # find the nearest center
                        min_dis = dis
                        this_vector_center = center
            # update centroids
                dic.get(this_vector_center).append(vector)

            centroids,delta_uk = update_centroids(dic)
            i+=1

        centroids = adapt_to_assumptions(centroids)
        return centroids

    except Exception:
        print("An Error Has Occurred")
        return

# update centroids
def update_centroids(dic):
    delta_uk = 0
    new_centroids = []
    for key in dic.keys():
        sum = tuple([0]*len(dic.get(key)[0]))
        for vector in dic.get(key):
            sum = tuple([sum[i]+vector[i] for i in range(len(vector))])
        new_centroids.append(tuple([sum[i]/len(dic.get(key)) for i in range(len(sum))]))
        delta_uk = max(delta_uk,d(key,new_centroids[-1]))
    return new_centroids,delta_uk


# update dict
def update_dict(centroids):
    dic = {}
    for i in range(len(centroids)):
        dic[centroids[i]] = []
    return dic


def d(p,q):
    sum = 0
    for i in range(len(p)):
        sum += (p[i]-q[i])**2
    return sum**0.5


def adapt_to_assumptions(centroids):
    new_centroids = []
    for centroid in centroids:
        new_centroids.append(tuple(['{:.4f}'.format(centroid[i]) for i in range(len(centroid))]))
    return new_centroids

# Press the green button in the gutter to run the script.
if __name__ == '__main__':

    print(k_means(3,'/Users/nadav/Downloads/tests/input_1.txt',600))
