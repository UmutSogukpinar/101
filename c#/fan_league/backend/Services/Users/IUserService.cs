using fan_league.Requests.Users;
using fan_league.Responses.Common;
using fan_league.Responses.Users;

namespace fan_league.Services.Users;

public interface IUserService
{
    Task<PagedResponse<UserResponse>> GetAllAsync(
        GetUsersRequest request,
        CancellationToken cancellationToken = default);
    Task<UserResponse> GetByIdAsync(Guid id, CancellationToken cancellationToken = default);
    Task<UserResponse> CreateAsync(CreateUserRequest request, CancellationToken cancellationToken = default);
    Task<UserResponse> UpdateAsync(Guid id, UpdateUserRequest request, CancellationToken cancellationToken = default);
    Task DeleteAsync(Guid id, CancellationToken cancellationToken = default);
}
