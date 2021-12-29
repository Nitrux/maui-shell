import Cutefish.Audio 1.0

SortFilterModel {
    property var filters: []
    property bool filterOutInactiveDevices: false

    function role(name) {
        return sourceModel.role(name);
    }

    // filterCallback: function(source_row, value) {
    //     var idx = sourceModel.index(source_row, 0);

    //     // Don't ever show the dummy output, that's silly
    //     var dummyOutputName = "auto_null"
    //     if (sourceModel.data(idx, sourceModel.role("Name")) === dummyOutputName) {
    //         return false;
    //     }

    //     // Optionally run the role-based filters
    //     if (filters.length > 0) {
    //         for (var i = 0; i < filters.length; ++i) {
    //             var filter = filters[i];
    //             if (sourceModel.data(idx, sourceModel.role(filter.role)) != filter.value) {
    //                 return false;
    //             }
    //         }
    //     }

    //     // Optionally exclude inactive devices
    //     if (filterOutInactiveDevices) {
    //         var ports = sourceModel.data(idx, sourceModel.role("PulseObject")).ports;
    //         if (ports.length === 1 && ports[0].availability == Port.Unavailable) {
    //             return false;
    //         }
    //     }
    //     return true;
    // }
}
