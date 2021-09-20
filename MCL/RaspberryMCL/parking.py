class Parking:
    '''
    Helper class to store actual parking state on an array
    '''
    def __init__(self,size):
        self.spots = [0]*size
        
    def parkEvent(self,spot, event):
        self.spots[spot] = event